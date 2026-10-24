# harbour-speakergain — the output volumes Sailfish OS keeps to itself.
#
# First knob: the alarm. Its loudness is bound by ngfd to the profiled key
# clock.alert.volume of the *general* profile, the platform ships it at 100,
# and no UI element writes it — so the alarm ignores both the ringtone slider
# and the volume keys. This app writes that key, and it says for every value
# whether a system update will keep it or throw it away.

TARGET = harbour-speakergain

CONFIG += sailfishapp
QT += core qml quick dbus

CONFIG += link_pkgconfig
PKGCONFIG += sailfishapp libpulse

isEmpty(VERSION): VERSION = 0.2.2
isEmpty(RELEASE): RELEASE = 1
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += APP_RELEASE=\\\"$$RELEASE\\\"

HEADERS += \
    src/speakergain.h

SOURCES += \
    src/harbour-speakergain.cpp \
    src/speakergain.cpp \
    src/diagnostics.cpp \
    src/softvol.cpp

# Software volume for USB headphones whose volume scale PulseAudio cannot use
# (src/softvol.cpp): the profile set and its paths, the rule naming the
# devices known to need it, the root helper that adds others, and the polkit
# rule that lets the user start exactly that helper.
softvolset.files = pulse/harbour-speakergain-softvol.conf
softvolset.path = /usr/share/pulseaudio/alsa-mixer/profile-sets
softvolpaths.files = pulse/harbour-speakergain-softvol-output.conf \
                     pulse/harbour-speakergain-softvol-input.conf
softvolpaths.path = /usr/share/pulseaudio/alsa-mixer/paths
softvolrules.files = pulse/91-harbour-speakergain-softvol.rules
# udev on Sailfish OS reads /lib, /etc and /run, not /usr/lib.
softvolrules.path = /lib/udev/rules.d
softvolunit.files = systemd/harbour-speakergain-softvol@.service
softvolunit.path = /usr/lib/systemd/system
softvolpolkit.files = polkit/50-harbour-speakergain.rules
softvolpolkit.path = /usr/share/polkit-1/rules.d
INSTALLS += softvolset softvolpaths softvolrules softvolunit softvolpolkit

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# English source strings + German (du-Form).
CONFIG += sailfishapp_i18n
TRANSLATIONS += translations/harbour-speakergain-de.ts \
                translations/harbour-speakergain-en.ts
lupdate_only {
    SOURCES += qml/*.qml qml/pages/*.qml qml/cover/*.qml
}

DISTFILES += \
    qml/harbour-speakergain.qml \
    qml/cover/CoverPage.qml \
    qml/pages/CarouselPage.qml \
    qml/pages/AlarmView.qml \
    qml/pages/StepsView.qml \
    qml/pages/OutputsView.qml \
    qml/pages/AboutPage.qml \
    qml/pages/DiagnosticsPage.qml \
    rpm/harbour-speakergain.spec \
    systemd/harbour-speakergain-softvol@.service \
    pulse/* \
    polkit/50-harbour-speakergain.rules \
    harbour-speakergain.desktop \
    translations/*.ts
