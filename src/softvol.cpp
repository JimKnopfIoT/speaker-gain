/*
  harbour-speakergain — softvol.cpp
  Copyright (C) 2026  harbour-speakergain contributors — GPLv3 or later.

  Software volume for USB headphones whose volume control reports a dB scale
  PulseAudio cannot use.

  The Bose Noise Cancelling Wired Earbuds claim their PCM control spans
  -25600..0 dB in 25600 steps; by the USB audio standard a step is 1/256 dB, so
  the real range is -100..0 dB. PulseAudio believes the claim and sets the
  volume in hardware: a stream at -40 dB moves the control by 40 steps, which
  is -0.16 dB, and the headphones stay at full scale whatever anyone sets.

  The package ships a profile set that keeps such a device's controls at full
  scale and leaves the volume to PulseAudio's software volume, and a udev rule
  that hands it to the devices known to need it. A device found on a phone is
  added by a helper that runs as root (a oneshot unit the polkit rule lets the
  user start): it writes the device's ids into a rule of its own and
  re-enumerates the device so PulseAudio loads it anew.
*/
#include "speakergain.h"

#include <QFile>
#include <QDir>
#include <QProcess>
#include <QRegularExpression>
#include <QStringList>
#include <QThread>

#include <unistd.h>

namespace {

const char *kShippedRules = "/lib/udev/rules.d/91-harbour-speakergain-softvol.rules";
const char *kLocalRules   = "/etc/udev/rules.d/91-harbour-speakergain-softvol-local.rules";
const char *kProfileSet   = "harbour-speakergain-softvol.conf";

QString readFile(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    return QString::fromUtf8(f.readAll());
}

// "05a7:4101" for every device a rules file names.
QStringList listedIds(const QString &rulesPath)
{
    QStringList out;
    const QRegularExpression re(QStringLiteral(
        "ATTRS\\{idVendor\\}==\"([0-9a-f]{4})\", ATTRS\\{idProduct\\}==\"([0-9a-f]{4})\""));
    QRegularExpressionMatchIterator it = re.globalMatch(readFile(rulesPath));
    while (it.hasNext()) {
        const QRegularExpressionMatch m = it.next();
        out << m.captured(1) + QLatin1Char(':') + m.captured(2);
    }
    return out;
}

bool writeLocalRules(const QStringList &ids)
{
    QString t;
    t += QStringLiteral(
        "# Written by harbour-speakergain: USB devices found on this phone whose\n"
        "# volume control reports a scale PulseAudio cannot use. PulseAudio sets\n"
        "# their volume in software. Removed with the app.\n\n"
        "SUBSYSTEM!=\"sound\", GOTO=\"speakergain_softvol_local_end\"\n"
        "ACTION!=\"change\", GOTO=\"speakergain_softvol_local_end\"\n"
        "KERNEL!=\"card*\", GOTO=\"speakergain_softvol_local_end\"\n\n");
    for (const QString &id : ids)
        t += QStringLiteral("ATTRS{idVendor}==\"%1\", ATTRS{idProduct}==\"%2\", ENV{PULSE_PROFILE_SET}=\"%3\"\n")
                 .arg(id.left(4), id.mid(5, 4), QLatin1String(kProfileSet));
    t += QStringLiteral("\nLABEL=\"speakergain_softvol_local_end\"\n");

    const QString tmp = QLatin1String(kLocalRules) + QStringLiteral(".new");
    QFile f(tmp);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return false;
    f.write(t.toUtf8());
    f.close();
    QFile::remove(QLatin1String(kLocalRules));
    return QFile::rename(tmp, QLatin1String(kLocalRules));
}

int runQuiet(const QString &program, const QStringList &args, int timeoutMs = 10000)
{
    QProcess p;
    p.start(program, args);
    if (!p.waitForFinished(timeoutMs)) {
        p.kill();
        p.waitForFinished(500);
        return -1;
    }
    return p.exitStatus() == QProcess::NormalExit ? p.exitCode() : -1;
}

// What unplugging and plugging in again would do: the kernel drops the device
// and enumerates it anew, udev runs the rules with the new entry, PulseAudio
// loads the card with the profile set the rule names.
void reenumerate(const QString &vendor, const QString &product)
{
    const QDir dir(QStringLiteral("/sys/bus/usb/devices"));
    for (const QString &d : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        const QString base = dir.filePath(d);
        if (readFile(base + QStringLiteral("/idVendor")).trimmed() != vendor
                || readFile(base + QStringLiteral("/idProduct")).trimmed() != product)
            continue;
        QFile a(base + QStringLiteral("/authorized"));
        if (!a.open(QIODevice::WriteOnly))
            continue;
        a.write("0");
        a.close();
        QThread::msleep(1000);
        if (a.open(QIODevice::WriteOnly)) {
            a.write("1");
            a.close();
        }
        qInfo("speakergain: re-enumerated USB device %s", qPrintable(d));
    }
}

} // namespace

int SpeakerGain::runSoftVolHelper(const QString &instance)
{
    if (geteuid() != 0) {
        qWarning("speakergain: --softvol needs root; the app starts it through systemd");
        return 1;
    }
    // Instance names come from the user side; nothing but the exact form
    // add|remove-VVVV-PPPP is taken.
    const QRegularExpression re(QStringLiteral("^(add|remove)-([0-9a-f]{4})-([0-9a-f]{4})$"));
    const QRegularExpressionMatch m = re.match(instance);
    if (!m.hasMatch()) {
        qWarning("speakergain: refusing instance '%s'", qPrintable(instance));
        return 2;
    }
    const bool add = m.captured(1) == QLatin1String("add");
    const QString vendor = m.captured(2), product = m.captured(3);
    const QString id = vendor + QLatin1Char(':') + product;

    QStringList ids = listedIds(QLatin1String(kLocalRules));
    if (add && !ids.contains(id))
        ids << id;
    if (!add)
        ids.removeAll(id);
    ids.sort();

    if (ids.isEmpty()) {
        QFile::remove(QLatin1String(kLocalRules));
    } else if (!writeLocalRules(ids)) {
        qWarning("speakergain: cannot write %s", kLocalRules);
        return 3;
    }
    qInfo("speakergain: %s %s", add ? "added" : "removed", qPrintable(id));

    runQuiet(QStringLiteral("udevadm"), QStringList() << QStringLiteral("control") << QStringLiteral("--reload"));
    reenumerate(vendor, product);
    return 0;
}

QString SpeakerGain::softVolumeSource(const QString &usbId)
{
    if (listedIds(QLatin1String(kShippedRules)).contains(usbId))
        return QStringLiteral("shipped");
    if (listedIds(QLatin1String(kLocalRules)).contains(usbId))
        return QStringLiteral("local");
    return QString();
}

bool SpeakerGain::setSoftVolume(const QString &usbId, bool on)
{
    m_error.clear();
    const QRegularExpression re(QStringLiteral("^([0-9a-f]{4}):([0-9a-f]{4})$"));
    const QRegularExpressionMatch m = re.match(usbId);
    if (!m.hasMatch()) {
        m_error = tr("Not a USB device id: %1").arg(usbId);
        emit changed();
        return false;
    }
    const QString unit = QStringLiteral("harbour-speakergain-softvol@%1-%2-%3.service")
                             .arg(on ? QStringLiteral("add") : QStringLiteral("remove"),
                                  m.captured(1), m.captured(2));
    // Blocks until the oneshot has run, re-enumeration included.
    const int rc = runQuiet(QStringLiteral("systemctl"),
                            QStringList() << QStringLiteral("--no-ask-password")
                                          << QStringLiteral("start") << unit, 20000);
    if (rc != 0)
        m_error = tr("The helper did not run (systemctl: %1).").arg(rc);
    emit changed();
    return rc == 0;
}
