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
PKGCONFIG += sailfishapp

isEmpty(VERSION): VERSION = 0.1.0
isEmpty(RELEASE): RELEASE = 1
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += APP_RELEASE=\\\"$$RELEASE\\\"

HEADERS += \
    src/speakergain.h

SOURCES += \
    src/harbour-speakergain.cpp \
    src/speakergain.cpp

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# English source strings + German (du-Form).
CONFIG += sailfishapp_i18n
TRANSLATIONS += translations/harbour-speakergain-de.ts
lupdate_only {
    SOURCES += qml/*.qml qml/pages/*.qml qml/cover/*.qml
}

DISTFILES += \
    qml/harbour-speakergain.qml \
    qml/cover/CoverPage.qml \
    qml/pages/MainPage.qml \
    qml/pages/RoutesPage.qml \
    qml/pages/AboutPage.qml \
    rpm/harbour-speakergain.spec \
    harbour-speakergain.desktop \
    translations/*.ts
