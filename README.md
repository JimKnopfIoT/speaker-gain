<img src="appicon/harbour-speakergain.svg" width="96" align="left" alt="Speaker Gain icon">

# Speaker Gain — the output volumes Sailfish OS keeps to itself

**The alarm rings at full volume and nothing quietens it. Bluetooth headphones
start louder than they should. Neither has a control anywhere.** This sets both,
without root, and says for every value whether a system update will keep it.

<br clear="left">

## The alarm

Not the ringtone slider, not the volume keys, not switching to Silent. That last
one is deliberate — an alarm is meant to wake you — but the rest is not:

1. The feedback daemon gives the alarm a volume entry of its own
   (`x-clock-alert-volume` in `/usr/share/ngfd/events.d/clock.ini`).
2. That entry is bound to a stored setting, hard-wired to the **general**
   profile whichever profile is actually active
   (`/usr/share/ngfd/plugins.d/50-streamrestore.ini`):

   ```ini
   role.x-ringtone-volume    = profile.current.ringing.alert.volume
   role.x-clock-alert-volume = profile.general.clock.alert.volume
   ```

3. The platform ships `clock.alert.volume` at **100**, and no part of the
   interface ever writes it.

Lowering the ringtone therefore does nothing — the mistake almost everyone makes
first. Verified on a Jolla Phone (2026), a Sony Xperia 10 III and a Gemini PDA:
the configuration files are identical on all three.

## Volume per output

Speaker, wired and both Bluetooth profiles each carry a volume of their own, per
kind of sound — media, calls, VoIP — and the system restores them when the
output changes. They live in your home directory, one entry per pairing of the
two.

Only the output **in use** can be set, because that is how the value is filed:
the headphones have to be connected to give them one.

### Bluetooth headphones that start too loud

This is the second thing the app is for. On Bluetooth the phone does not turn
the sound down itself: it hands the figure to the headphones over AVRCP and
holds **their** own volume control wide open. Everything you hear is therefore
set by the per-route value alone — and a route that has never played before
starts from the platform's table, 25 dB below unity for media and 15 dB for
calls. Ten decibels louder for a call than for music, on headphones already at
maximum, and picked without knowing anything about the headphones attached.

The fix is one slider:

1. connect the headphones and play something,
2. open Speaker Gain → **Volume per output**,
3. lower **Media · Bluetooth, music** to taste.

It stays. The value lives in your home directory, the phone restores it every
time those headphones come back, and a system update does not touch it. Calls
have their own value on the same output; set it separately if a call is louder
than the music, which by default it is.

## Volume steps, read only

The volume keys do not move a percentage. They walk a list of fixed levels, and
each output has lists of its own — one for calls, one for VoIP, one for the
rest. The page shows how many levels there are, how far down they reach, and the
widest gap between two neighbours, which is what one press costs at its coarsest
point. On the phones here that ranges from 6 levels for a call on the
loudspeaker to 20 for music on headphones.

Those tables belong to a package and an update replaces them, so this app reads
them and leaves them alone.

## Will the value survive?

Every value says where it is stored and what a system update does to it, worked
out from the package database at runtime rather than from a table in the source:

- a value in your own home directory is kept — nothing owns it;
- a package file marked as configuration is kept, with the update's version
  beside it;
- a package file not so marked is overwritten without a backup and without a
  notice.

## How it works

No root, no system file touched. The alarm volume is one stored number in your
own settings, written through the same interface the system uses itself and read
back afterwards rather than assumed.

The per-output volumes are written the way the platform writes them: a stream
carrying that kind of sound is opened, its volume is set, and the audio system
files it under the output in use — the same move
[Mic Gain](https://github.com/JimKnopfIoT/harbour-micgain) makes for recording,
mirrored to playback. No sample is ever played through that stream, so nothing
is heard.

## Installing

```sh
pkcon install-local harbour-speakergain-*.aarch64.rpm
```

Sandboxing is disabled on purpose: the app talks to the settings daemon over the
session bus and reads the audio policy's own configuration files, neither of
which is reachable from inside a sandbox.

## Building

With the Sailfish Platform SDK:

```sh
mb2 -t SailfishOS-5.0.0.62-aarch64 build
```

## Status

0.1.0. The alarm slider is verified on all three devices named above. The
per-output part is verified on the Jolla Phone (2026) and the Xperia 10 III; it
builds and installs for armv7hl but has not been tried on hardware there.

## Licence

GPL-3.0-or-later. No warranty — but every change is one number in your own
settings, and each one can be put back.
