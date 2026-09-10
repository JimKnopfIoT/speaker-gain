<img src="appicon/harbour-speakergain.svg" width="96" align="left" alt="Speaker Gain icon">

# Speaker Gain — the output volumes Sailfish OS keeps to itself

**One slider for the alarm, and an honest answer for every value: does an
update keep it, or throw it away?** No root, nothing patched, revertible.

<br clear="left">

## The problem

The alarm rings at full volume and nothing quietens it — not the ringtone
slider, not the volume keys, not switching to Silent. That is not a bug in one
device; it is how the platform is wired:

1. The feedback daemon gives the alarm a volume entry of its own
   (`x-clock-alert-volume` in `/usr/share/ngfd/events.d/clock.ini`).
2. That entry is bound to a stored setting — and hard-wired to the **general**
   profile, whichever profile is actually active
   (`/usr/share/ngfd/plugins.d/50-streamrestore.ini`):

   ```ini
   role.x-ringtone-volume    = profile.current.ringing.alert.volume
   role.x-clock-alert-volume = profile.general.clock.alert.volume
   ```

3. The platform ships `clock.alert.volume` at **100**, and no part of the
   interface ever writes it.

The volume keys do not reach it either: they drive the main volume classes
(call and media), and the alarm is not one of them.

Verified on a Jolla Phone (2026) and an Xperia 10 III — the configuration
files are byte for byte the same on both.

## What the app does

- **Alarm volume, 0–100**, written through the same interface the system uses
  itself, read back afterwards rather than assumed. A button puts the shipped
  value back.
- **Volume steps per output** — speaker, wired headphones, Bluetooth for music
  (A2DP), Bluetooth for calls (HFP/HSP), line out. Each output walks a fixed
  list of levels rather than a percentage, one list for calls, one for VoIP,
  one for the rest. The app shows how many levels there are, how far down they
  reach, and the largest gap between two neighbours — which is what one press
  of the key costs at its coarsest point. Read only.
- **Will it survive?** Every value says where it is stored and what a system
  update does to it, derived from the package database at runtime: a value in
  your own home directory is kept; a file that belongs to a package and is not
  marked as configuration is replaced without a backup and without a notice.

## Building

With the Sailfish Platform SDK:

```sh
mb2 -t SailfishOS-5.0.0.62-aarch64 build
```

## Licence

GPL-3.0-or-later. No warranty — but the whole change is one number in your own
settings, and the reset button restores the shipped value at any time.
