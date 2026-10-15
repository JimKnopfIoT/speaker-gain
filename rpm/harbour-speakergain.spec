# harbour-speakergain — the output volumes Sailfish OS keeps to itself.
#
# Neutral packaging metadata — no personal identifiers.
Name:       harbour-speakergain
Summary:    Set the alarm volume the system hides
Version:    0.2.0
Release:    2
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

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_prefix}/lib/systemd/user/%{name}-apply.service

%changelog
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
