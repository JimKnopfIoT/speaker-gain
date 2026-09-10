/*
  harbour-speakergain — speakergain.cpp
  Copyright (C) 2026  harbour-speakergain contributors — GPLv3 or later.
*/
#include "speakergain.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>
#include <QTextStream>

namespace {

const char *kService   = "com.nokia.profiled";
const char *kPath      = "/com/nokia/profiled";
const char *kInterface = "com.nokia.profiled";

// The alarm hangs off the *general* profile, not the active one: ngfd binds
// it as "role.x-clock-alert-volume = profile.general.clock.alert.volume"
// (/usr/share/ngfd/plugins.d/50-streamrestore.ini). Switching to Silent
// therefore does not quieten the alarm, which is deliberate — and it is also
// why this app always writes "general" and never the current profile.
const char *kAlarmProfile = "general";
const char *kAlarmKey     = "clock.alert.volume";
const char *kRingKey      = "ringing.alert.volume";

const char *kDefaultsFile = "/etc/profiled/50.sailfish_default.ini";
const char *kMainVolumeDir = "/var/lib/nemo-pulseaudio-parameters/algs/mainvolume";

QString readAll(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    return QString::fromUtf8(f.readAll());
}

// rpm answers unprivileged; it reads /var/lib/rpm. Kept short-lived and
// bounded so a hung database cannot freeze the page.
QString runRpm(const QStringList &args)
{
    QProcess p;
    p.start(QStringLiteral("rpm"), args);
    if (!p.waitForFinished(4000)) {
        p.kill();
        p.waitForFinished(500);
        return QString();
    }
    if (p.exitStatus() != QProcess::NormalExit || p.exitCode() != 0)
        return QString();
    return QString::fromUtf8(p.readAllStandardOutput()).trimmed();
}

// "0:-6000,1:-1400,2:-800,3:-450,4:-190,5:0" -> the steps in millibel.
QList<int> parseSteps(const QString &value)
{
    QList<int> out;
    const QStringList parts = value.split(QLatin1Char(','), QString::SkipEmptyParts);
    for (const QString &p : parts) {
        const int colon = p.indexOf(QLatin1Char(':'));
        if (colon < 0)
            continue;
        bool ok = false;
        const int mb = p.mid(colon + 1).trimmed().toInt(&ok);
        if (ok)
            out.append(mb);
    }
    return out;
}

// What one curve says about itself. No thresholds of ours: the step count,
// the two ends, and the largest jump between neighbours are all read off the
// numbers. The largest jump is the interesting one — it is what a single
// press of the volume key does at its coarsest point.
QVariantMap describeCurve(const QString &raw)
{
    QVariantMap m;
    const QList<int> steps = parseSteps(raw);
    m.insert(QStringLiteral("count"), steps.size());
    if (steps.isEmpty())
        return m;

    int lowest = steps.first(), highest = steps.first();
    int gap = 0, gapFrom = -1;
    for (int i = 0; i < steps.size(); ++i) {
        lowest = qMin(lowest, steps.at(i));
        highest = qMax(highest, steps.at(i));
        if (i > 0) {
            const int d = qAbs(steps.at(i) - steps.at(i - 1));
            if (d > gap) { gap = d; gapFrom = i - 1; }
        }
    }
    m.insert(QStringLiteral("lowDb"), lowest / 100.0);
    m.insert(QStringLiteral("highDb"), highest / 100.0);
    m.insert(QStringLiteral("gapDb"), gap / 100.0);
    m.insert(QStringLiteral("gapFrom"), gapFrom);
    return m;
}

} // namespace

SpeakerGain::SpeakerGain(QObject *parent)
    : QObject(parent)
{
    refresh();
}

QString SpeakerGain::profiledGet(const QString &profile, const QString &key) const
{
    QDBusInterface iface(QLatin1String(kService), QLatin1String(kPath),
                         QLatin1String(kInterface), QDBusConnection::sessionBus());
    if (!iface.isValid())
        return QString();
    const QDBusReply<QString> r = iface.call(QStringLiteral("get_value"), profile, key);
    return r.isValid() ? r.value() : QString();
}

bool SpeakerGain::profiledSet(const QString &profile, const QString &key, const QString &value)
{
    QDBusInterface iface(QLatin1String(kService), QLatin1String(kPath),
                         QLatin1String(kInterface), QDBusConnection::sessionBus());
    if (!iface.isValid()) {
        m_error = tr("profiled is not answering on the session bus.");
        return false;
    }
    const QDBusReply<bool> r = iface.call(QStringLiteral("set_value"), profile, key, value);
    if (!r.isValid()) {
        m_error = r.error().message();
        return false;
    }
    if (!r.value()) {
        m_error = tr("profiled refused the value.");
        return false;
    }
    return true;
}

void SpeakerGain::refresh()
{
    QDBusInterface iface(QLatin1String(kService), QLatin1String(kPath),
                         QLatin1String(kInterface), QDBusConnection::sessionBus());
    m_available = iface.isValid();
    if (m_available) {
        const QDBusReply<QString> cur = iface.call(QStringLiteral("get_profile"));
        m_profile = cur.isValid() ? cur.value() : QString();

        bool ok = false;
        const int a = profiledGet(QLatin1String(kAlarmProfile), QLatin1String(kAlarmKey)).toInt(&ok);
        m_alarm = ok ? a : -1;
        ok = false;
        const int r = profiledGet(QLatin1String(kAlarmProfile), QLatin1String(kRingKey)).toInt(&ok);
        m_ring = ok ? r : -1;
    } else {
        m_error = tr("profiled is not answering on the session bus.");
        m_alarm = m_ring = -1;
        m_profile.clear();
    }
    emit changed();
}

bool SpeakerGain::setAlarmVolume(int percent)
{
    m_error.clear();
    percent = qBound(0, percent, 100);
    if (!profiledSet(QLatin1String(kAlarmProfile), QLatin1String(kAlarmKey),
                     QString::number(percent))) {
        emit changed();
        return false;
    }

    // Read back rather than trust the reply.
    bool ok = false;
    const int now = profiledGet(QLatin1String(kAlarmProfile), QLatin1String(kAlarmKey)).toInt(&ok);
    m_alarm = ok ? now : -1;
    const bool good = ok && now == percent;
    if (!good)
        m_error = tr("Written, but reading it back gave a different value.");
    emit changed();
    return good;
}

int SpeakerGain::defaultValue(const QString &key) const
{
    // The shipped file lists the keys once per profile section; the general
    // profile's block is the one the alarm reads.
    const QString text = readAll(QLatin1String(kDefaultsFile));
    if (text.isEmpty())
        return -1;

    QString section;
    const QStringList lines = text.split(QLatin1Char('\n'));
    int fallback = -1;
    for (const QString &raw : lines) {
        const QString line = raw.trimmed();
        if (line.startsWith(QLatin1Char('[')) && line.endsWith(QLatin1Char(']'))) {
            section = line.mid(1, line.size() - 2).trimmed().toLower();
            continue;
        }
        const int eq = line.indexOf(QLatin1Char('='));
        if (eq < 0 || line.left(eq).trimmed() != key)
            continue;
        bool ok = false;
        const int v = line.mid(eq + 1).trimmed().toInt(&ok);
        if (!ok)
            continue;                       // the type line ("INTEGER 0-100")
        if (section.contains(QStringLiteral("general")))
            return v;
        if (fallback < 0)
            fallback = v;
    }
    return fallback;
}

QVariantMap SpeakerGain::storage(const QString &path) const
{
    QVariantMap m;
    m.insert(QStringLiteral("path"), path);
    const QFileInfo fi(path);
    m.insert(QStringLiteral("exists"), fi.exists());

    const QString home = QDir::homePath();
    const bool inHome = !home.isEmpty() && path.startsWith(home + QLatin1Char('/'));

    if (inHome) {
        // No package owns anything under the user's home, so there is nothing
        // for an update to replace. Saves the rpm call as well.
        m.insert(QStringLiteral("package"), QString());
        m.insert(QStringLiteral("isConfig"), false);
        m.insert(QStringLiteral("verdict"), QStringLiteral("user"));
        m.insert(QStringLiteral("headline"), tr("Survives updates"));
        m.insert(QStringLiteral("detail"),
                 tr("The value is in your home directory and belongs to no package. "
                    "A restart keeps it, and so does a system update."));
        return m;
    }

    const QString pkg = runRpm(QStringList() << QStringLiteral("-qf") << path);
    if (pkg.isEmpty() || pkg.contains(QStringLiteral("not owned"))) {
        m.insert(QStringLiteral("package"), QString());
        m.insert(QStringLiteral("isConfig"), false);
        m.insert(QStringLiteral("verdict"), QStringLiteral("unknown"));
        m.insert(QStringLiteral("headline"), tr("No package owns this file"));
        m.insert(QStringLiteral("detail"),
                 tr("Nothing in the package database claims this path, so no update "
                    "is going to replace it. Whoever put it there is not the package "
                    "manager."));
        return m;
    }

    const QString pkgName = pkg.split(QLatin1Char('\n')).first().trimmed();
    m.insert(QStringLiteral("package"), pkgName);

    const QString configs = runRpm(QStringList() << QStringLiteral("-qc") << pkgName);
    const bool isConfig = configs.split(QLatin1Char('\n')).contains(path);
    m.insert(QStringLiteral("isConfig"), isConfig);

    if (isConfig) {
        m.insert(QStringLiteral("verdict"), QStringLiteral("config"));
        m.insert(QStringLiteral("headline"), tr("Kept as configuration"));
        m.insert(QStringLiteral("detail"),
                 tr("The file belongs to %1 and is marked as configuration. An update "
                    "keeps your edit and puts its own version next to it.").arg(pkgName));
    } else {
        m.insert(QStringLiteral("verdict"), QStringLiteral("replaced"));
        m.insert(QStringLiteral("headline"), tr("An update throws this away"));
        m.insert(QStringLiteral("detail"),
                 tr("The file belongs to %1 and is not marked as configuration. The next "
                    "update of that package overwrites it — no backup copy, no notice.")
                     .arg(pkgName));
    }
    return m;
}

QVariantMap SpeakerGain::alarmStorage() const
{
    return storage(QDir::homePath() + QStringLiteral("/.config/profiled/custom.ini"));
}

QVariantList SpeakerGain::routes() const
{
    // Which mode uses which table is a symlink under modes/; several modes
    // share one file, and that sharing is worth showing — "bluetooth for
    // calls" and "bluetooth for music" are two different curves.
    QVariantList out;
    // Braces, not parentheses: with a type-name argument the parenthesised
    // form declares a function instead of an object.
    const QDir algs = QDir(QString::fromLatin1(kMainVolumeDir));
    if (!algs.exists())
        return out;

    const QDir modes(QStringLiteral("/var/lib/nemo-pulseaudio-parameters/modes"));
    QMap<QString, QStringList> users;
    const QStringList modeNames = modes.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QString &mode : modeNames) {
        const QFileInfo link(modes.filePath(mode) + QStringLiteral("/mainvolume"));
        if (!link.exists())
            continue;
        users[QFileInfo(link.canonicalFilePath()).fileName()].append(mode);
    }

    const QStringList files = algs.entryList(QDir::Files, QDir::Name);
    for (const QString &name : files) {
        const QString path = algs.filePath(name);
        const QString text = readAll(path);
        if (text.isEmpty())
            continue;

        QVariantMap r;
        r.insert(QStringLiteral("name"), name);
        r.insert(QStringLiteral("path"), path);
        r.insert(QStringLiteral("modes"), users.value(name));

        QVariantMap curves;
        const QRegularExpression re(
            QStringLiteral("x-nemo\\.mainvolume\\.([a-z0-9-]+)\\s*=\\s*\"([^\"]*)\""));
        QRegularExpressionMatchIterator it = re.globalMatch(text);
        while (it.hasNext()) {
            const QRegularExpressionMatch m = it.next();
            const QString which = m.captured(1);
            if (which == QLatin1String("high-volume-step")) {
                r.insert(QStringLiteral("highVolumeStep"), m.captured(2));
                continue;
            }
            curves.insert(which, describeCurve(m.captured(2)));
        }
        r.insert(QStringLiteral("curves"), curves);
        out.append(r);
    }
    return out;
}
