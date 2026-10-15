# harbour-speakergain — the output volumes Sailfish OS keeps to itself.
#
# Neutral packaging metadata — no personal identifiers.
Name:       harbour-speakergain
Summary:    Set the alarm volume the system hides
Version:    0.2.1
Release:    1
# Neutral build host so built RPMs carry no real hostname/domain.
%define _buildhost reproducible-builder
License:    GPL-3.0-or-later
URL:        https://github.com/JimKnopfIoT/speaker-gain
Source0:    %{name}-%{version}.tar.bz2
Vendor:     harbour-speakergain contributors
Packager:   harbour-speakergain contributors

Requires:   sailfishsilica-qt5

BuildRequires: pkgconfig(sailfishapp)
BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5DBus)
BuildRequires: pkgconfig(Qt5Qml)
BuildRequires: pkgconfig(Qt5Quick)
BuildRequires: pkgconfig(libpulse)
BuildRequires: desktop-file-utils
BuildRequires: qt5-qttools-linguist

%description
The alarm on Sailfish OS has a loudness of its own. The feedback daemon binds
it to a stored setting of the general profile, the system ships that setting
at full volume, and no part of the interface writes it — so the alarm ignores
the ringtone slider, the volume keys and the Silent profile alike. This app
sets that one number, through the same interface the system uses itself: no
root, no patched file, revertible with one button.

It also reads the volume step tables of every output — speaker, wired, both
Bluetooth profiles, line out — and shows what a single press of the volume key
does on each. Those tables belong to a package and an update replaces them, so
the app reports that instead of editing them. Every value it shows carries the
same verdict: does a system update keep this, or throw it away.

USB headphones whose volume control reports a dB scale the system cannot use
(Bose Noise Cancelling Wired Earbuds among them) stay at full volume whatever
is set. For those the package brings a PulseAudio profile that sets their
volume in software, and a helper the user can start to add further devices.

%prep
%setup -q

%build
%qmake5 VERSION=%{version} RELEASE=%{release}
%make_build

%install
%qmake5_install
strip %{buildroot}%{_bindir}/%{name} || :

desktop-file-install --delete-original \
    --dir %{buildroot}%{_datadir}/applications \
    %{buildroot}%{_datadir}/applications/*.desktop

%post
# The rule naming the USB devices that get software volume.
udevadm control --reload >/dev/null 2>&1 || :
systemctl daemon-reload >/dev/null 2>&1 || :

%postun
if [ "$1" = 0 ]; then
    # Devices the app added on this phone go with it.
    rm -f /etc/udev/rules.d/91-%{name}-softvol-local.rules
fi
udevadm control --reload >/dev/null 2>&1 || :
systemctl daemon-reload >/dev/null 2>&1 || :

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_prefix}/lib/systemd/user/%{name}-apply.service
%{_prefix}/lib/systemd/system/%{name}-softvol@.service
/lib/udev/rules.d/91-%{name}-softvol.rules
%{_datadir}/pulseaudio/alsa-mixer/profile-sets/%{name}-softvol.conf
%{_datadir}/pulseaudio/alsa-mixer/paths/%{name}-softvol-output.conf
%{_datadir}/pulseaudio/alsa-mixer/paths/%{name}-softvol-input.conf
%{_datadir}/polkit-1/rules.d/50-%{name}.rules

%changelog
* Thu Oct 15 2026 harbour-speakergain contributors 0.2.1-1
- USB headphones whose volume control reports a dB scale the system cannot
  use now follow the volume. The Bose Noise Cancelling Wired Earbuds claim
  their control spans 25600 dB, PulseAudio believes it, and every setting
  moved them by a fraction of a dB: they stayed at full volume. The package
  brings a PulseAudio profile that keeps such a device's control at full scale
  and sets the volume in software, and a udev rule naming the devices known to
  need it. Others are offered on the outputs page; a helper with system rights,
  which a polkit rule lets the user start and nothing else, adds them.
- About -> Diagnostics: a compact report of how the phone's sound is put
  together - route, sinks, streams, the volume controls of USB cards and what
  udev handed PulseAudio for them - with what looks wrong spelled out. Serial
  numbers, Bluetooth addresses and names, the host name and the apps playing
  are left out. Copy or save it; nothing is sent. Also on the terminal as
  harbour-speakergain --diagnostics.
* Thu Oct 15 2026 harbour-speakergain contributors 0.2.0-2
- The three views sit side by side in a carousel: volume steps on the left,
  the alarm in the middle, the sliders per output on the right. The pull-down
  menu holds only About.
* Thu Oct 15 2026 harbour-speakergain contributors 0.2.0-1
- Take the output in use from the audio policy's routing decision instead of
  the sinks, which keep stale route names (Jolla Phone (2026): the sliders sat
  on wired headphones while the speaker played).
- Sliders for every output the system knows. The one in use takes the value at
  once; any other gets it the moment it is next used, through a helper that
  waits for that and quits.
- Sliders for outputs with nothing stored yet, at the system's starting figure.
- The own volume of Bluetooth headphones and USB sound cards.
- The About page shows the version again.
* Thu Sep 10 2026 harbour-speakergain contributors 0.1.0-1
- Initial version: slider for the alarm volume (profiled key
  clock.alert.volume of the general profile), the shipped default and a reset
  button, per-output volume step tables read only, and a durability verdict
  for every value derived from the package database.
