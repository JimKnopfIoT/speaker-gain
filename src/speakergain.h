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

public:
    explicit SpeakerGain(QObject *parent = nullptr);

    bool available() const { return m_available; }
    int alarmVolume() const { return m_alarm; }
    int ringVolume() const { return m_ring; }
    QString currentProfile() const { return m_profile; }
    QString lastError() const { return m_error; }

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
    // (wired), "bta2dp" (Bluetooth music) or empty when it cannot be told.
    Q_INVOKABLE QString activeRoute() const;

    // Set the volume of one role for the route in use, the way the platform
    // stores it itself: open a playback stream carrying that role, set its
    // volume, let it go. Writes nothing to any file of the system. Blocks for
    // well under a second; the database behind it is flushed a few seconds
    // later, so a read-back right afterwards still shows the old value.
    Q_INVOKABLE bool setRoleVolume(const QString &role, int percent);

    // One entry per output route (speaker, wired, A2DP, mono BT, line out)
    // with its volume step tables. Read only — these files belong to a
    // package and an update overwrites them.
    Q_INVOKABLE QVariantList routes() const;

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
