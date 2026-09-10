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
#include <QProcessEnvironment>
#include <QRegularExpression>
#include <QTextStream>
#include <QThread>

#include <pulse/pulseaudio.h>

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
// Output that gets parsed must not depend on the user's language. pactl
// translates its own field names - on a German device "Sink #0" reads "Ziel #0"
// and "Active Port:" reads "Aktiver Port:" - and rpm translates "is not owned
// by any package" just as happily. A parser looking for the English words then
// finds nothing, and the page shows no output at all: no route, no slider.
void useCLocale(QProcess &p)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert(QStringLiteral("LC_ALL"), QStringLiteral("C"));
    env.insert(QStringLiteral("LANG"), QStringLiteral("C"));
    env.remove(QStringLiteral("LANGUAGE"));
    p.setProcessEnvironment(env);
}

QString runRpm(const QStringList &args)
{
    QProcess p;
    useCLocale(p);
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

// ---------------------------------------------------------------------------
// The volumes PulseAudio keeps for itself
//
// Two databases in ~/.config/pulse hold them, both in the "simple" format:
// a run of records, each u32 key length, key, u32 data length, data.
//
//   *-stream-volumes.simple        one entry per role or named volume, the
//                                  payload a PulseAudio tagstruct
//   *-x-maemo-route-volumes.simple one entry per role *and route*, the payload
//                                  a plain struct with alignment padding
//
// The decoders below were checked against values the audio policy states in
// plain text (/usr/share/ngfd/plugins.d/50-streamrestore.ini): the fixed
// entries x-static-volume-full/-moderate/-silent and the two tone volumes came
// out as 100, 40, 0, 50 and 50 percent — which is what that file sets.
// ---------------------------------------------------------------------------
namespace {

struct DbEntry { QString key; QByteArray data; };

quint32 leU32(const QByteArray &b, int off)
{
    return (quint8)b.at(off) | ((quint32)(quint8)b.at(off + 1) << 8)
         | ((quint32)(quint8)b.at(off + 2) << 16) | ((quint32)(quint8)b.at(off + 3) << 24);
}

quint32 beU32(const QByteArray &b, int off)
{
    return ((quint32)(quint8)b.at(off) << 24) | ((quint32)(quint8)b.at(off + 1) << 16)
         | ((quint32)(quint8)b.at(off + 2) << 8) | (quint8)b.at(off + 3);
}

QList<DbEntry> readDb(const QString &path)
{
    QList<DbEntry> out;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return out;
    const QByteArray b = f.readAll();
    int off = 0;
    while (off + 4 <= b.size()) {
        const quint32 klen = leU32(b, off);
        if (klen == 0 || klen > 200 || off + 4 + (int)klen + 4 > b.size())
            break;
        const QString key = QString::fromUtf8(b.mid(off + 4, klen));
        off += 4 + klen;
        const quint32 dlen = leU32(b, off);
        if (off + 4 + (int)dlen > b.size())
            break;
        out.append({ key, b.mid(off + 4, dlen) });
        off += 4 + dlen;
    }
    return out;
}

// A PulseAudio tagstruct, walked tag by tag until the volume turns up. Walking
// it beats searching for the volume tag: the byte that marks a volume can just
// as well sit inside a device name.
double tagVolume(const QByteArray &d, bool *ok)
{
    *ok = false;
    int i = 0;
    while (i < d.size()) {
        const quint8 t = (quint8)d.at(i);
        if (t == 0x42) { i += 2; }                                  // u8
        else if (t == 0x30 || t == 0x31) { i += 1; }                 // boolean
        else if (t == 0x6d) {                                        // channel map
            if (i + 1 >= d.size()) return 0;
            i += 2 + (quint8)d.at(i + 1);
        } else if (t == 0x76) {                                      // cvolume
            if (i + 1 >= d.size()) return 0;
            const int n = (quint8)d.at(i + 1);
            if (n < 1 || i + 2 + 4 * n > d.size()) return 0;
            double sum = 0;
            for (int c = 0; c < n; ++c)
                sum += beU32(d, i + 2 + 4 * c);
            *ok = true;
            return sum / n / 65536.0 * 100.0;
        } else if (t == 0x74) {                                      // string
            const int nul = d.indexOf('\0', i + 1);
            if (nul < 0) return 0;
            i = nul + 1;
        } else if (t == 0x4e) { i += 1; }                            // empty string
        else if (t == 0x4c) { i += 5; }                              // u32
        else return 0;
    }
    return 0;
}

// The route database is not a tagstruct: u32 version, u8 channels, three bytes
// the compiler left as padding, then one u32 per channel.
double rawVolume(const QByteArray &d, bool *ok)
{
    *ok = false;
    if (d.size() < 12 || leU32(d, 0) != 4)
        return 0;
    const int n = (quint8)d.at(4);
    if (n < 1 || n > 8 || d.size() < 8 + 4 * n)
        return 0;
    double sum = 0;
    for (int c = 0; c < n; ++c) {
        const quint32 v = leU32(d, 8 + 4 * c);
        if (v > 0x20000)
            return 0;
        sum += v;
    }
    *ok = true;
    return sum / n / 65536.0 * 100.0;
}

QString pulseDir()
{
    return QDir::homePath() + QStringLiteral("/.config/pulse");
}

QString dbPath(const QString &suffix)
{
    const QDir d(pulseDir());
    const QStringList m = d.entryList(QStringList() << (QStringLiteral("*") + suffix), QDir::Files);
    return m.isEmpty() ? QString() : d.filePath(m.first());
}

// Role names as the tables spell them, with what they actually carry.
QString roleLabel(const QString &role)
{
    if (role == QLatin1String("x-maemo"))
        return SpeakerGain::tr("Media");
    if (role == QLatin1String("phone"))
        return SpeakerGain::tr("Calls");
    if (role == QLatin1String("voip"))
        return SpeakerGain::tr("VoIP");
    return role;
}

QString routeLabel(const QString &route)
{
    if (route == QLatin1String("ihf"))
        return SpeakerGain::tr("Speaker");
    if (route == QLatin1String("hp"))
        return SpeakerGain::tr("Wired headphones");
    if (route == QLatin1String("hs"))
        return SpeakerGain::tr("Wired headset");
    if (route == QLatin1String("bta2dp"))
        return SpeakerGain::tr("Bluetooth, music");
    if (route.startsWith(QLatin1String("btmono")))
        return SpeakerGain::tr("Bluetooth, calls");
    if (route == QLatin1String("lineout"))
        return SpeakerGain::tr("Line out");
    if (route == QLatin1String("usbaudio"))
        return SpeakerGain::tr("USB audio");
    return route;
}

} // namespace

QString SpeakerGain::activeRoute() const
{
    // Asking PulseAudio which sink is running answers it for the case that
    // matters — something is playing. With everything idle the port of the
    // built-in card still says where sound would go.
    QProcess p;
    useCLocale(p);
    p.start(QStringLiteral("pactl"), QStringList() << QStringLiteral("list") << QStringLiteral("sinks"));
    if (!p.waitForFinished(2500))
        return QString();
    const QStringList lines = QString::fromUtf8(p.readAllStandardOutput()).split(QLatin1Char('\n'));

    QString name, port, state, best;
    auto flush = [&]() {
        if (name.isEmpty())
            return;
        QString route;
        if (name.contains(QStringLiteral("bluez")) && name.contains(QStringLiteral("a2dp")))
            route = QStringLiteral("bta2dp");
        else if (name.contains(QStringLiteral("bluez")))
            route = QStringLiteral("btmono");
        else if (port.contains(QStringLiteral("headphone")))
            route = QStringLiteral("hp");
        else if (port.contains(QStringLiteral("headset")))
            route = QStringLiteral("hs");
        else if (port.contains(QStringLiteral("speaker")))
            route = QStringLiteral("ihf");
        if (route.isEmpty())
            return;
        if (state == QLatin1String("RUNNING") || best.isEmpty())
            best = route;
    };

    for (const QString &raw : lines) {
        const QString l = raw.trimmed();
        if (l.startsWith(QStringLiteral("Sink #"))) {
            flush();
            name.clear(); port.clear(); state.clear();
        } else if (l.startsWith(QStringLiteral("Name:")))
            name = l.mid(5).trimmed();
        else if (l.startsWith(QStringLiteral("Active Port:")))
            port = l.mid(12).trimmed();
        else if (l.startsWith(QStringLiteral("State:")))
            state = l.mid(6).trimmed();
    }
    flush();
    return best;
}

QVariantList SpeakerGain::outputVolumes() const
{
    QVariantList out;
    const QString cur = activeRoute();

    const QString routeFile = dbPath(QStringLiteral("-x-maemo-route-volumes.simple"));
    const QString streamFile = dbPath(QStringLiteral("-stream-volumes.simple"));

    const QString prefix = QStringLiteral("sink-input-by-media-role:");
    for (const DbEntry &e : readDb(routeFile)) {
        if (!e.key.startsWith(prefix))
            continue;
        const QString rest = e.key.mid(prefix.size());
        const int colon = rest.indexOf(QLatin1Char(':'));
        if (colon < 0)
            continue;
        bool ok = false;
        const double pc = rawVolume(e.data, &ok);
        if (!ok)
            continue;

        const QString role = rest.left(colon), route = rest.mid(colon + 1);
        QVariantMap m;
        m.insert(QStringLiteral("role"), role);
        m.insert(QStringLiteral("route"), route);
        m.insert(QStringLiteral("roleLabel"), roleLabel(role));
        m.insert(QStringLiteral("routeLabel"), routeLabel(route));
        m.insert(QStringLiteral("percent"), pc);
        m.insert(QStringLiteral("current"), !cur.isEmpty() && route == cur);
        m.insert(QStringLiteral("source"), routeFile);
        out.append(m);
    }

    // The named volumes that are not tied to a route — the alert entries and
    // the fixed ones the policy writes itself.
    for (const DbEntry &e : readDb(streamFile)) {
        if (!e.key.startsWith(QStringLiteral("x-")))
            continue;
        bool ok = false;
        const double pc = tagVolume(e.data, &ok);
        if (!ok)
            continue;
        QVariantMap m;
        m.insert(QStringLiteral("role"), e.key);
        m.insert(QStringLiteral("route"), QString());
        m.insert(QStringLiteral("roleLabel"), e.key);
        m.insert(QStringLiteral("routeLabel"), QString());
        m.insert(QStringLiteral("percent"), pc);
        m.insert(QStringLiteral("current"), false);
        m.insert(QStringLiteral("source"), streamFile);
        out.append(m);
    }
    return out;
}

// ---------------------------------------------------------------------------
// Writing a role's volume
//
// The same move Mic Gain makes for recording, mirrored to playback: open a
// stream that carries the role, set its volume, let it go. PulseAudio's
// stream-restore module keeps the value under that role's name and, on this
// platform, under the name of the route in use as well — which is why the
// setting is per output and why the output has to be the active one.
//
// No sample is ever written to the stream, so nothing is heard.
// ---------------------------------------------------------------------------
namespace {

struct PaCtx {
    pa_threaded_mainloop *ml = nullptr;
    pa_context *ctx = nullptr;
    pa_stream *stream = nullptr;
    bool opOk = false;
};

void paCtxState(pa_context *, void *ud)
{
    pa_threaded_mainloop_signal(static_cast<pa_threaded_mainloop *>(ud), 0);
}

void paStreamState(pa_stream *, void *ud)
{
    pa_threaded_mainloop_signal(static_cast<pa_threaded_mainloop *>(ud), 0);
}

void paSuccess(pa_context *, int success, void *ud)
{
    PaCtx *c = static_cast<PaCtx *>(ud);
    c->opOk = success != 0;
    pa_threaded_mainloop_signal(c->ml, 0);
}

void paWait(PaCtx &c, pa_operation *o)
{
    if (!o)
        return;
    while (pa_operation_get_state(o) == PA_OPERATION_RUNNING)
        pa_threaded_mainloop_wait(c.ml);
    pa_operation_unref(o);
}

void paTeardown(PaCtx &c, bool locked)
{
    if (!c.ml)
        return;
    if (!locked)
        pa_threaded_mainloop_lock(c.ml);
    if (c.stream) {
        pa_stream_disconnect(c.stream);
        pa_stream_unref(c.stream);
        c.stream = nullptr;
    }
    if (c.ctx) {
        pa_context_disconnect(c.ctx);
        pa_context_unref(c.ctx);
        c.ctx = nullptr;
    }
    pa_threaded_mainloop_unlock(c.ml);
    pa_threaded_mainloop_stop(c.ml);
    pa_threaded_mainloop_free(c.ml);
    c.ml = nullptr;
}

} // namespace

bool SpeakerGain::setRoleVolume(const QString &role, int percent)
{
    m_error.clear();
    percent = qBound(0, percent, 100);

    PaCtx c;
    c.ml = pa_threaded_mainloop_new();
    if (!c.ml) {
        m_error = tr("PulseAudio could not be reached.");
        emit changed();
        return false;
    }

    pa_proplist *pl = pa_proplist_new();
    pa_proplist_sets(pl, PA_PROP_APPLICATION_NAME, "Speaker Gain");
    pa_proplist_sets(pl, PA_PROP_MEDIA_ROLE, role.toUtf8().constData());

    c.ctx = pa_context_new_with_proplist(pa_threaded_mainloop_get_api(c.ml),
                                         "Speaker Gain", pl);
    pa_context_set_state_callback(c.ctx, paCtxState, c.ml);
    pa_threaded_mainloop_start(c.ml);
    pa_threaded_mainloop_lock(c.ml);

    if (pa_context_connect(c.ctx, nullptr, PA_CONTEXT_NOFLAGS, nullptr) < 0) {
        m_error = tr("PulseAudio refused the connection: %1")
                      .arg(QString::fromUtf8(pa_strerror(pa_context_errno(c.ctx))));
        pa_proplist_free(pl);
        paTeardown(c, true);
        emit changed();
        return false;
    }
    for (;;) {
        const pa_context_state_t st = pa_context_get_state(c.ctx);
        if (st == PA_CONTEXT_READY)
            break;
        if (!PA_CONTEXT_IS_GOOD(st)) {
            m_error = tr("PulseAudio dropped the connection: %1")
                          .arg(QString::fromUtf8(pa_strerror(pa_context_errno(c.ctx))));
            pa_proplist_free(pl);
            paTeardown(c, true);
            emit changed();
            return false;
        }
        pa_threaded_mainloop_wait(c.ml);
    }

    pa_sample_spec spec;
    spec.format = PA_SAMPLE_S16LE;
    spec.rate = 44100;
    spec.channels = 2;

    c.stream = pa_stream_new_with_proplist(c.ctx, "Speaker Gain", &spec, nullptr, pl);
    pa_proplist_free(pl);
    pa_stream_set_state_callback(c.stream, paStreamState, c.ml);

    // Connected but never fed: the stream exists for the policy to place and
    // for the volume to attach to, and it plays nothing at all.
    if (pa_stream_connect_playback(c.stream, nullptr, nullptr,
                                   PA_STREAM_NOFLAGS, nullptr, nullptr) < 0) {
        m_error = tr("The stream could not be opened: %1")
                      .arg(QString::fromUtf8(pa_strerror(pa_context_errno(c.ctx))));
        paTeardown(c, true);
        emit changed();
        return false;
    }
    for (;;) {
        const pa_stream_state_t st = pa_stream_get_state(c.stream);
        if (st == PA_STREAM_READY)
            break;
        if (!PA_STREAM_IS_GOOD(st)) {
            m_error = tr("The stream could not be opened: %1")
                          .arg(QString::fromUtf8(pa_strerror(pa_context_errno(c.ctx))));
            paTeardown(c, true);
            emit changed();
            return false;
        }
        pa_threaded_mainloop_wait(c.ml);
    }

    const uint32_t idx = pa_stream_get_index(c.stream);
    pa_cvolume v;
    pa_cvolume_init(&v);
    pa_cvolume_set(&v, spec.channels,
                   (pa_volume_t)(PA_VOLUME_NORM * (double)percent / 100.0));
    c.opOk = false;
    paWait(c, pa_context_set_sink_input_volume(c.ctx, idx, &v, paSuccess, &c));
    const bool ok = c.opOk;

    pa_threaded_mainloop_unlock(c.ml);
    // Give the module the moment it needs to notice the stream's volume before
    // the stream disappears again.
    QThread::msleep(300);
    paTeardown(c, false);

    if (!ok)
        m_error = tr("PulseAudio did not accept the volume.");
    emit changed();
    return ok;
}
