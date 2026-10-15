/*
  harbour-speakergain — speakergain.h
  Copyright (C) 2026  harbour-speakergain contributors — GPLv3 or later.

  Everything the app knows about output volumes:

  - the alert volumes profiled holds (the alarm above all), read and written
    over the session bus,
  - the per-route volume step tables the audio policy ships, read only,
  - and for every one of them, whether the value lives in the user's home or
    in a package's file — which decides whether a system update keeps it.
*/
#ifndef SPEAKERGAIN_H
#define SPEAKERGAIN_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

class SpeakerGain : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool available READ available NOTIFY changed)
    Q_PROPERTY(int alarmVolume READ alarmVolume NOTIFY changed)
    Q_PROPERTY(int ringVolume READ ringVolume NOTIFY changed)
    Q_PROPERTY(QString currentProfile READ currentProfile NOTIFY changed)
    Q_PROPERTY(QString lastError READ lastError NOTIFY changed)
    // Show this app in English whatever the phone is set to - for screenshots,
    // and for anyone who would rather read the original wording. Takes effect
    // when the app is started again; nothing outside this app is touched.
    Q_PROPERTY(bool forceEnglish READ forceEnglish WRITE setForceEnglish NOTIFY changed)

public:
    explicit SpeakerGain(QObject *parent = nullptr);

    bool available() const { return m_available; }
    int alarmVolume() const { return m_alarm; }
    int ringVolume() const { return m_ring; }
    QString currentProfile() const { return m_profile; }
    QString lastError() const { return m_error; }
    bool forceEnglish() const;
    void setForceEnglish(bool on);

    // Re-read everything from profiled.
    Q_INVOKABLE void refresh();

    // Write the alarm volume (0-100) and read it back. Returns true only when
    // the read-back matches: profiled answers the D-Bus call before it has
    // written its file, so "the call succeeded" is not the same as "the value
    // is set".
    Q_INVOKABLE bool setAlarmVolume(int percent);

    // The platform's shipped default for a key, straight out of
    // /etc/profiled/50.sailfish_default.ini — what "reset" means.
    Q_INVOKABLE int defaultValue(const QString &key) const;

    // Where a value is stored and what a system update does to it.
    // Keys: path, exists, package, isConfig, verdict, headline, detail.
    // verdict: "user" | "config" | "replaced" | "unknown"
    Q_INVOKABLE QVariantMap storage(const QString &path) const;

    // The alarm's own storage, without the caller having to know the path.
    Q_INVOKABLE QVariantMap alarmStorage() const;

    // The volumes PulseAudio keeps per role and per output route, decoded from
    // the two databases in the user's own config directory. Keys per entry:
    // role, route, percent, current (route is the one in use), source.
    Q_INVOKABLE QVariantList outputVolumes() const;

    // Which route the device is playing on right now: "ihf" (speaker), "hp"
    // (wired), "bta2dp" (Bluetooth music), "usbaudio" and so on, exactly as
    // the volume database spells it, or empty when it cannot be told.
    Q_INVOKABLE QString activeRoute() const;

    // A route name in words, for headers.
    Q_INVOKABLE QString routeName(const QString &route) const;

    // Set the volume of one role for the route in use, the way the platform
    // stores it itself: open a playback stream carrying that role, set its
    // volume, let it go. Writes nothing to any file of the system. Blocks for
    // well under a second; the database behind it is flushed a few seconds
    // later, so a read-back right afterwards still shows the old value.
    Q_INVOKABLE bool setRoleVolume(const QString &role, int percent);

    // Outputs that keep a volume of their own on top of the per-route one:
    // Bluetooth headphones (the figure is sent to them over AVRCP) and USB or
    // other sound cards PulseAudio drives directly. The phone's own droid
    // sinks are left out - their volume is not the place the platform sets
    // anything. Keys: name, label, kind ("bluetooth" | "usb" | "other"),
    // percent, running; for USB also usbId ("vvvv:pppp"), scaleBroken (its
    // volume control claims an impossible dB range), softVolume (PulseAudio
    // sets its volume in software) and softVolumeSource (see there).
    Q_INVOKABLE QVariantList deviceSinks() const;

    // Set one of those outputs' own volume. Not stored by the system: it holds
    // while the output stays connected.
    Q_INVOKABLE bool setSinkVolume(const QString &sink, int percent);

    // One entry per output route (speaker, wired, A2DP, mono BT, line out)
    // with its volume step tables. Read only — these files belong to a
    // package and an update overwrites them.
    Q_INVOKABLE QVariantList routes() const;

    // Every output route the system knows - the modes the audio parameters
    // ship plus whatever the database already holds - with one entry per role
    // each. Keys per route: route, label, current, entries; per entry: role,
    // roleLabel, percent, stored, pending (-1 when nothing is waiting).
    Q_INVOKABLE QVariantList allRoutes() const;

    // Set a role's volume on any route. The route in use takes it at once.
    // Any other route cannot be written while it is not in use - PulseAudio
    // keeps that database in memory and only ever files the current route -
    // so the value waits, and a small helper applies it the moment the route
    // comes into use and then quits. Returns false only on a real failure.
    Q_INVOKABLE bool setRouteVolume(const QString &route, const QString &role, int percent);

    // Drop a waiting value.
    Q_INVOKABLE void clearPending(const QString &route, const QString &role);

    // A compact report of how the phone's sound is put together - route,
    // sinks, streams, the volume controls of external cards - with what looks
    // wrong spelled out. Serial numbers, Bluetooth addresses and names and the
    // host name are masked. Lives in diagnostics.cpp.
    Q_INVOKABLE QString diagnosticReport() const;

    // Write such a report to Documents; returns the path, empty on failure.
    Q_INVOKABLE QString saveDiagnosticReport(const QString &text) const;

    // Software volume for a USB device whose volume control reports a dB
    // scale PulseAudio cannot use (softvol.cpp). usbId is "vvvv:pppp". Starts
    // the root helper through systemd - the polkit rule allows exactly that -
    // which lists the device and re-enumerates it. Blocks for a few seconds.
    Q_INVOKABLE bool setSoftVolume(const QString &usbId, bool on);

    // Where a device's software volume comes from: "shipped" (the package
    // names it), "local" (added on this phone) or empty.
    static QString softVolumeSource(const QString &usbId);

    // Whether a USB sound card's playback volume claims a dB range no device
    // has (more than 200 dB), from /proc/asound/cardN/usbmixer. Fills in the
    // control's name and the claimed span when it does.
    static bool usbScaleBroken(const QString &card, QString *control = nullptr,
                               double *spanDb = nullptr);

    // The root side of setSoftVolume: "harbour-speakergain --softvol
    // add-vvvv-pppp", run by harbour-speakergain-softvol@.service.
    static int runSoftVolHelper(const QString &instance);

    // The helper: "harbour-speakergain --apply-pending". Waits for route
    // changes, applies what is waiting, quits when nothing is left.
    static int runApplyHelper(int argc, char *argv[]);

signals:
    void changed();

private:
    QString profiledGet(const QString &profile, const QString &key) const;
    bool profiledSet(const QString &profile, const QString &key, const QString &value);

    bool m_available = false;
    int m_alarm = -1;
    int m_ring = -1;
    QString m_profile;
    QString m_error;
};

#endif // SPEAKERGAIN_H
