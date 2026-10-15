/*
  harbour-speakergain — diagnostics.cpp
  Copyright (C) 2026  harbour-speakergain contributors — GPLv3 or later.

  A compact report of how this phone's sound is put together, for a user to
  pass on when a slider does not do what it says. It holds what it takes to
  fix a fault from afar - the route the policy picked, the sinks with their
  flags, which stream plays where, the volume controls of external sound
  cards with their dB scale, the module arguments that decide how a card is
  driven - and it says plainly what looks wrong.

  It holds nothing that points to a person: serial numbers, Bluetooth
  addresses and the phone's host name are masked, Bluetooth devices appear by
  kind and not by the name their owner gave them, and streams by role and not
  by application. Nothing is sent anywhere; the user copies or saves the text
  and decides where it goes.
*/
#include "speakergain.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <algorithm>
#include <QMap>
#include <QProcess>
#include <QProcessEnvironment>
#include <QRegularExpression>
#include <QSet>
#include <QStandardPaths>
#include <QSysInfo>
#include <QTextStream>

namespace {

QString readText(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    return QString::fromUtf8(f.readAll()).trimmed();
}

// Field names of pactl and rpm are translated; the report is parsed and read
// by people of any language, so it is taken in the C locale.
QString run(const QString &program, const QStringList &args)
{
    QProcess p;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert(QStringLiteral("LC_ALL"), QStringLiteral("C"));
    env.insert(QStringLiteral("LANG"), QStringLiteral("C"));
    env.remove(QStringLiteral("LANGUAGE"));
    p.setProcessEnvironment(env);
    p.start(program, args);
    if (!p.waitForFinished(3000)) {
        p.kill();
        p.waitForFinished(500);
        return QString();
    }
    return QString::fromUtf8(p.readAllStandardOutput());
}

QString osRelease(const QString &path, const QString &key)
{
    for (const QString &l : readText(path).split(QLatin1Char('\n'))) {
        if (l.startsWith(key + QLatin1Char('=')))
            return l.mid(key.size() + 1).remove(QLatin1Char('"'));
    }
    return QString();
}

// One "Sink #4" / "Sink Input #1" / "Card #0" block of pactl list.
struct Block {
    QString index;
    QMap<QString, QString> fields;  // "State" -> "RUNNING"
    QMap<QString, QString> props;   // "device.bus" -> "usb"
};

QList<Block> blocks(const QString &text, const QString &header)
{
    QList<Block> out;
    for (const QString &l : text.split(QLatin1Char('\n'))) {
        if (l.startsWith(header)) {
            Block b;
            b.index = l.mid(header.size()).trimmed();
            out.append(b);
            continue;
        }
        if (out.isEmpty())
            continue;
        if (l.startsWith(QLatin1String("\t\t"))) {
            const QString t = l.trimmed();
            const int eq = t.indexOf(QLatin1String(" = "));
            if (eq > 0)
                out.last().props.insert(t.left(eq), t.mid(eq + 3).remove(QLatin1Char('"')));
        } else if (l.startsWith(QLatin1Char('\t'))) {
            const QString t = l.trimmed();
            const int colon = t.indexOf(QLatin1Char(':'));
            if (colon > 0 && !out.last().fields.contains(t.left(colon)))
                out.last().fields.insert(t.left(colon), t.mid(colon + 1).trimmed());
        }
    }
    return out;
}

// "front-left: 14119 /  22% / -40.00 dB,   front-right: ..." -> "22% -40.00 dB"
QString shortVolume(const QString &v)
{
    const QRegularExpression re(QStringLiteral("(\\d+)%\\s*/\\s*(-?[\\d.]+ dB|-inf dB)"));
    const QRegularExpressionMatch m = re.match(v);
    if (m.hasMatch())
        return m.captured(1) + QStringLiteral("% ") + m.captured(2);
    const QRegularExpression pc(QStringLiteral("(\\d+)%"));
    const QRegularExpressionMatch p = pc.match(v);
    return p.hasMatch() ? p.captured(1) + QLatin1Char('%') : v;
}

// One volume control of a USB sound card, as the kernel reports it in
// /proc/asound/cardN/usbmixer. dB figures there are hundredths of a dB.
struct UsbControl {
    QString name;
    long min = 0, max = 0, dBmin = 0, dBmax = 0;
};

QList<UsbControl> usbControls(const QString &usbmixer)
{
    QList<UsbControl> out;
    const QRegularExpression nameRe(QStringLiteral("Control: name=\"([^\"]+)\""));
    const QRegularExpression volRe(QStringLiteral(
        "Volume: min=(-?\\d+), max=(-?\\d+), dBmin=(-?\\d+), dBmax=(-?\\d+)"));
    QString name;
    for (const QString &l : usbmixer.split(QLatin1Char('\n'))) {
        const QRegularExpressionMatch n = nameRe.match(l);
        if (n.hasMatch()) {
            name = n.captured(1);
            continue;
        }
        const QRegularExpressionMatch v = volRe.match(l);
        if (v.hasMatch() && name.endsWith(QLatin1String("Volume"))) {
            UsbControl c;
            c.name = name;
            c.min = v.captured(1).toLong();
            c.max = v.captured(2).toLong();
            c.dBmin = v.captured(3).toLong();
            c.dBmax = v.captured(4).toLong();
            out.append(c);
        }
    }
    return out;
}

// What can identify the owner, collected before anything is written: the
// serial numbers USB devices and PulseAudio report, Bluetooth addresses (by
// pattern) and the host name.
class Masker {
public:
    void addSerial(const QString &s)
    {
        if (s.size() >= 6 && s.contains(QRegularExpression(QStringLiteral("\\d")))
                && QRegularExpression(QStringLiteral("^[A-Za-z0-9._-]+$")).match(s).hasMatch())
            m_serials.insert(s);
    }
    void setHost(const QString &h) { m_host = h; }

    QString operator()(QString t) const
    {
        // Longest first, so a serial that contains another is masked whole.
        QStringList s = m_serials.values();
        std::sort(s.begin(), s.end(), [](const QString &a, const QString &b) {
            return a.size() > b.size();
        });
        for (const QString &x : s)
            t.replace(x, QStringLiteral("SERIAL"));
        t.replace(QRegularExpression(QStringLiteral("([0-9A-Fa-f]{2}[:_]){5}[0-9A-Fa-f]{2}")),
                  QStringLiteral("XX:XX:XX:XX:XX:XX"));
        if (m_host.size() >= 3)
            t.replace(m_host, QStringLiteral("HOST"), Qt::CaseInsensitive);
        return t;
    }

private:
    QSet<QString> m_serials;
    QString m_host;
};

} // namespace

bool SpeakerGain::usbScaleBroken(const QString &card, QString *control, double *spanDb)
{
    if (card.isEmpty())
        return false;
    const QString dir = QStringLiteral("/proc/asound/card") + card;
    for (const UsbControl &c : usbControls(readText(dir + QStringLiteral("/usbmixer")))) {
        if (!c.name.contains(QLatin1String("Playback")) || c.max <= c.min)
            continue;
        const double span = (c.dBmax - c.dBmin) / 100.0;
        if (span > 200.0) {
            if (control)
                *control = c.name;
            if (spanDb)
                *spanDb = span;
            return true;
        }
    }
    return false;
}

QString SpeakerGain::diagnosticReport() const
{
    QString out;
    QTextStream o(&out);
    QStringList findings;

    const QString sinksText = run(QStringLiteral("pactl"), QStringList() << QStringLiteral("list") << QStringLiteral("sinks"));
    const QString inputsText = run(QStringLiteral("pactl"), QStringList() << QStringLiteral("list") << QStringLiteral("sink-inputs"));
    const QString cardsText = run(QStringLiteral("pactl"), QStringList() << QStringLiteral("list") << QStringLiteral("cards"));
    const QString modulesText = run(QStringLiteral("pactl"), QStringList() << QStringLiteral("list") << QStringLiteral("modules") << QStringLiteral("short"));
    const QString infoText = run(QStringLiteral("pactl"), QStringList() << QStringLiteral("info"));

    Masker mask;
    mask.setHost(QSysInfo::machineHostName());
    for (const QString &d : QDir(QStringLiteral("/sys/bus/usb/devices")).entryList(QDir::Dirs | QDir::NoDotAndDotDot))
        mask.addSerial(readText(QStringLiteral("/sys/bus/usb/devices/") + d + QStringLiteral("/serial")));
    const QList<Block> sinks = blocks(sinksText, QStringLiteral("Sink #"));
    const QList<Block> cards = blocks(cardsText, QStringLiteral("Card #"));
    for (const QList<Block> &list : { sinks, cards }) {
        for (const Block &b : list) {
            mask.addSerial(b.props.value(QStringLiteral("device.serial")));
            const QString udevId = b.props.value(QStringLiteral("udev.id"));
            const int us = udevId.lastIndexOf(QLatin1Char('_'));
            if (us >= 0)
                mask.addSerial(udevId.mid(us + 1).section(QLatin1Char('-'), 0, 0));
        }
    }

    // --- The phone ---------------------------------------------------------
    o << "Speaker Gain diagnostics " << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n";
    o << "app " << APP_VERSION << '-' << APP_RELEASE
      << " | " << osRelease(QStringLiteral("/etc/sailfish-release"), QStringLiteral("NAME"))
      << ' ' << osRelease(QStringLiteral("/etc/sailfish-release"), QStringLiteral("VERSION_ID"))
      << " | " << osRelease(QStringLiteral("/etc/hw-release"), QStringLiteral("NAME"))
      << " | kernel " << QSysInfo::kernelVersion() << "\n";
    for (const QString &l : infoText.split(QLatin1Char('\n'))) {
        if (l.startsWith(QLatin1String("Server Version:")))
            o << "pulseaudio " << l.mid(15).trimmed() << "\n";
    }
    const QString route = activeRoute();
    o << "route in use: " << (route.isEmpty() ? QStringLiteral("(unknown)") : route) << "\n";

    // --- Modules that decide how outputs are driven -------------------------
    o << "\nmodules:\n";
    for (const QString &l : modulesText.split(QLatin1Char('\n'))) {
        const QStringList f = l.split(QLatin1Char('\t'));
        if (f.size() < 2)
            continue;
        const QString &name = f.at(1);
        if (!name.contains(QLatin1String("alsa")) && !name.contains(QLatin1String("udev"))
                && !name.contains(QLatin1String("droid-card")) && !name.contains(QLatin1String("bluez"))
                && !name.contains(QLatin1String("policy")) && !name.contains(QLatin1String("stream-restore"))
                && !name.contains(QLatin1String("mainvolume")))
            continue;
        QString args = f.size() > 2 ? f.at(2).trimmed() : QString();
        // A card's name and udev id say nothing the sink list does not; the
        // switches after them are what matters.
        args.remove(QRegularExpression(QStringLiteral("(name|card_name|device_id)=\"[^\"]*\"\\s*")));
        args.remove(QRegularExpression(QStringLiteral("card_properties=\"[^\"]*\"")));
        o << "  " << name << (args.isEmpty() ? QString() : QStringLiteral("  ") + args.simplified()) << "\n";
    }

    // --- Sinks --------------------------------------------------------------
    o << "\nsinks:\n";
    QMap<QString, QString> sinkName;  // index -> name
    for (const Block &b : sinks) {
        const QString name = b.fields.value(QStringLiteral("Name"));
        sinkName.insert(b.index, name);
        const QString driver = b.fields.value(QStringLiteral("Driver"));
        const QString bus = b.props.value(QStringLiteral("device.bus"));
        const bool bt = name.startsWith(QLatin1String("bluez")) || bus == QLatin1String("bluetooth");
        o << "  #" << b.index << ' ' << name << "\n";
        o << "     " << driver;
        if (!bus.isEmpty())
            o << " | bus " << bus;
        const QString vid = b.props.value(QStringLiteral("device.vendor.id"));
        const QString pid = b.props.value(QStringLiteral("device.product.id"));
        if (!vid.isEmpty() && !pid.isEmpty())
            o << " | usb " << vid << ':' << pid;
        if (!bt) {
            // A Bluetooth device's description is the name its owner gave it.
            const QString product = b.props.value(QStringLiteral("device.product.name"));
            if (!product.isEmpty())
                o << " | " << product;
        }
        if (b.props.contains(QStringLiteral("alsa.card")))
            o << " | alsa card " << b.props.value(QStringLiteral("alsa.card"));
        o << "\n     " << b.fields.value(QStringLiteral("State"))
          << " | vol " << shortVolume(b.fields.value(QStringLiteral("Volume")))
          << " | base " << shortVolume(b.fields.value(QStringLiteral("Base Volume")))
          << " | mute " << b.fields.value(QStringLiteral("Mute"))
          << "\n     flags " << b.fields.value(QStringLiteral("Flags")).simplified();
        const QString port = b.fields.value(QStringLiteral("Active Port"));
        if (!port.isEmpty())
            o << " | port " << port;
        const QString mode = b.props.value(QStringLiteral("x-maemo.mode"));
        if (!mode.isEmpty())
            o << " | x-maemo.mode " << mode;
        if (bt) {
            const QString codec = b.props.value(QStringLiteral("bluetooth.codec"));
            if (!codec.isEmpty())
                o << " | codec " << codec;
        }
        o << "\n";
    }

    // --- Streams ------------------------------------------------------------
    o << "\nstreams:\n";
    const QList<Block> inputs = blocks(inputsText, QStringLiteral("Sink Input #"));
    QMap<QString, int> streamsOn;
    for (const Block &b : inputs) {
        const QString sink = b.fields.value(QStringLiteral("Sink"));
        streamsOn[sink]++;
        QString role = b.props.value(QStringLiteral("media.role"));
        o << "  #" << b.index << " -> sink #" << sink
          << " | role " << (role.isEmpty() ? QStringLiteral("-") : role)
          << " | vol " << shortVolume(b.fields.value(QStringLiteral("Volume")))
          << " | corked " << b.fields.value(QStringLiteral("Corked")) << "\n";
    }
    if (inputs.isEmpty())
        o << "  none playing\n";

    // --- ALSA cards ---------------------------------------------------------
    // Cards PulseAudio drives itself, through a sink of its own. A USB card
    // missing here is driven by the Android audio HAL (pulseaudio-modules-droid
    // with usb_devices=true, as on the Jolla Phone (2026)): it plays through a
    // droid sink, whose volume is set in software, and the card's own volume
    // control is left alone.
    QSet<QString> paCards;
    for (const Block &b : sinks) {
        if (b.props.contains(QStringLiteral("alsa.card")))
            paCards.insert(b.props.value(QStringLiteral("alsa.card")));
    }
    o << "\nalsa cards:\n";
    const QString cardList = readText(QStringLiteral("/proc/asound/cards"));
    const QRegularExpression cardRe(QStringLiteral("^\\s*(\\d+) \\[([^\\]]+)\\]:\\s*(.*)$"));
    for (const QString &l : cardList.split(QLatin1Char('\n'))) {
        const QRegularExpressionMatch m = cardRe.match(l);
        if (!m.hasMatch())
            continue;
        const QString n = m.captured(1);
        const QString dir = QStringLiteral("/proc/asound/card") + n;
        const QString usbid = readText(dir + QStringLiteral("/usbid"));
        o << "  " << n << ' ' << m.captured(2).trimmed() << " | " << m.captured(3).trimmed();
        if (!usbid.isEmpty())
            o << " | usb " << usbid;
        o << "\n";
        if (usbid.isEmpty())
            continue;
        if (!paCards.contains(n))
            o << "     driven by the Android audio HAL, not by a PulseAudio sink of its own\n";
        // What udev handed PulseAudio for this card: PULSE_PROFILE_SET shows
        // whether a rule picked it up. Only these keys - the others carry the
        // serial number.
        QStringList udevProps;
        for (const QString &l : run(QStringLiteral("udevadm"),
                                    QStringList() << QStringLiteral("info") << QStringLiteral("-q")
                                                  << QStringLiteral("property")
                                                  << dir.mid(QStringLiteral("/proc/asound/").size())
                                                         .prepend(QStringLiteral("/sys/class/sound/")))
                                     .split(QLatin1Char('\n'))) {
            if (l.startsWith(QLatin1String("PULSE_")) || l.startsWith(QLatin1String("SOUND_")))
                udevProps << l.trimmed();
        }
        o << "     udev: " << (udevProps.isEmpty() ? QStringLiteral("(no PULSE_/SOUND_ properties)")
                                                     : udevProps.join(QLatin1Char(' '))) << "\n";
        for (const UsbControl &c : usbControls(readText(dir + QStringLiteral("/usbmixer")))) {
            const double span = (c.dBmax - c.dBmin) / 100.0;
            o << "     " << c.name << ": raw " << c.min << ".." << c.max
              << " | " << QString::number(c.dBmin / 100.0, 'f', 2) << ".."
              << QString::number(c.dBmax / 100.0, 'f', 2) << " dB\n";
            if (c.name.contains(QLatin1String("Playback")) && c.max > c.min) {
                if (span > 200.0 && !paCards.contains(n)) {
                    findings << QStringLiteral("USB card %1 (%2): '%3' claims a range of %4 dB, but the "
                                               "Android audio HAL drives this card and PulseAudio sets its "
                                               "volume in software, so the scale does not matter here.")
                                    .arg(n, usbid.trimmed().toLower(), c.name, QString::number(span, 'f', 0));
                } else if (span > 200.0) {
                    const QString id = usbid.trimmed().toLower();
                    const QString src = softVolumeSource(id);
                    findings << QStringLiteral("USB card %1 (%2): '%3' claims a range of %4 dB. "
                                               "PulseAudio takes that scale at its word, so a hardware "
                                               "volume barely moves the control. Software volume: %5.")
                                    .arg(n, id, c.name, QString::number(span, 'f', 0),
                                         src.isEmpty() ? QStringLiteral("not set up")
                                                       : QStringLiteral("listed (%1)").arg(src));
                }
                else if (span <= 0.0)
                    findings << QStringLiteral("USB card %1 (%2): '%3' reports no dB scale; PulseAudio "
                                               "maps the volume linearly onto the raw steps.")
                                    .arg(n, usbid, c.name);
            }
        }
    }

    // --- What the app sees ---------------------------------------------------
    o << "\nstored route volumes:\n";
    int stored = 0;
    for (const QVariant &v : outputVolumes()) {
        const QVariantMap m = v.toMap();
        if (!m.value(QStringLiteral("stored")).toBool())
            continue;
        const QString r = m.value(QStringLiteral("route")).toString();
        o << "  " << m.value(QStringLiteral("role")).toString()
          << (r.isEmpty() ? QString() : QLatin1Char(':') + r)
          << ' ' << QString::number(m.value(QStringLiteral("percent")).toDouble(), 'f', 1) << "%\n";
        ++stored;
    }
    if (stored == 0)
        o << "  none\n";

    o << "\npending:\n";
    int pending = 0;
    for (const QVariant &rv : allRoutes()) {
        const QVariantMap r = rv.toMap();
        for (const QVariant &ev : r.value(QStringLiteral("entries")).toList()) {
            const QVariantMap e = ev.toMap();
            const int p = e.value(QStringLiteral("pending")).toInt();
            if (p >= 0) {
                o << "  " << r.value(QStringLiteral("route")).toString() << '/'
                  << e.value(QStringLiteral("role")).toString() << ' ' << p << "%\n";
                ++pending;
            }
        }
    }
    if (pending == 0)
        o << "  none\n";

    // --- Findings -------------------------------------------------------------
    for (const Block &b : sinks) {
        const QString name = b.fields.value(QStringLiteral("Name"));
        if (!name.startsWith(QLatin1String("alsa_output.")) && !name.startsWith(QLatin1String("bluez")))
            continue;
        if (b.fields.value(QStringLiteral("State")) == QLatin1String("RUNNING") && streamsOn.value(b.index) == 0)
            findings << QStringLiteral("Sink #%1 is running with no stream on it.").arg(b.index);
    }
    if (!inputs.isEmpty()) {
        bool onExternal = false;
        for (const Block &b : inputs) {
            const QString n = sinkName.value(b.fields.value(QStringLiteral("Sink")));
            if (n.startsWith(QLatin1String("alsa_output.")) || n.startsWith(QLatin1String("bluez")))
                onExternal = true;
        }
        bool externalPresent = false;
        for (const Block &b : sinks) {
            const QString n = b.fields.value(QStringLiteral("Name"));
            if (n.startsWith(QLatin1String("alsa_output.")) || n.startsWith(QLatin1String("bluez")))
                externalPresent = true;
        }
        if (externalPresent && !onExternal)
            findings << QStringLiteral("An external output is connected, but every stream plays on the phone's own sinks.");
    }
    o << "\nfindings:\n";
    if (findings.isEmpty())
        o << "  nothing conspicuous\n";
    for (const QString &f : findings)
        o << "  - " << f << "\n";

    o.flush();
    return mask(out);
}

QString SpeakerGain::saveDiagnosticReport(const QString &text) const
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (dir.isEmpty() || !QDir(dir).exists())
        dir = QDir::homePath();
    const QString path = dir + QStringLiteral("/speakergain-diagnostics-")
                         + QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-HHmmss"))
                         + QStringLiteral(".txt");
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return QString();
    f.write(text.toUtf8());
    return path;
}
