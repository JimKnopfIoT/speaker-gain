import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page
    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: col.height + Theme.paddingLarge

        Column {
            id: col
            width: page.width
            spacing: Theme.paddingMedium

            PageHeader { title: qsTr("About Speaker Gain") }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                text: qsTr("Version %1").arg(Qt.application.version)
                color: Theme.secondaryColor
            }

            SectionHeader { text: qsTr("What is going on with the alarm") }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("The feedback daemon gives the alarm a volume of its own, under the name x-clock-alert-volume, and binds that name to a stored setting called clock.alert.volume. It reads that setting from the general profile — always the general one, whichever profile is active. The system ships it at 100 and offers no way to change it, so the alarm is as loud as the device goes while the ringtone obeys its slider.")
            }
            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: qsTr("Measured on a Jolla Phone (2026) and an Xperia 10 III; the configuration files are identical on both, so this is how the platform behaves and not a fault of one device.")
            }

            SectionHeader { text: qsTr("What this app changes") }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("One stored number, in your own settings, through the same interface the system uses itself. No root, nothing patched, no file of the system touched. Set it back to the shipped value and nothing of this app is left behind.")
            }

            SectionHeader { text: qsTr("Language") }

            // A language for this window only. Screenshots for a listing have
            // to be in English, and nobody should have to change the language
            // of their phone to take them: that closes every app and leaves the
            // device in a state its owner did not ask for. The switch sets the
            // locale of this process alone, before the application exists.
            TextSwitch {
                checked: speakergain.forceEnglish
                text: qsTr("Show this app in English")
                description: qsTr("Regardless of the language the phone is set to. Takes effect the next time the app is started, and changes nothing outside this app.")
                onClicked: speakergain.forceEnglish = !speakergain.forceEnglish
            }

            SectionHeader { text: qsTr("Licence") }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: qsTr("GPL-3.0-or-later. No warranty — but the whole change is one number in your own settings, and the button next to the slider puts the shipped value back.")
            }
        }
        VerticalScrollDecorator {}
    }
}
