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

isEmpty(VERSION): VERSION = 0.2.0
isEmpty(RELEASE): RELEASE = 1
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += APP_RELEASE=\\\"$$RELEASE\\\"

HEADERS += \
    src/speakergain.h

SOURCES += \
    src/harbour-speakergain.cpp \
    src/speakergain.cpp

# The helper that applies a volume for an output once it comes into use.
systemd.files = systemd/harbour-speakergain-apply.service
systemd.path = /usr/lib/systemd/user
INSTALLS += systemd

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
    rpm/harbour-speakergain.spec \
    systemd/harbour-speakergain-apply.service \
    harbour-speakergain.desktop \
    translations/*.ts
