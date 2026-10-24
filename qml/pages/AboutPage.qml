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
            // Asked about, and written about: say plainly what keeps running
            // and what gets system rights, with the names to look them up by.
            SectionHeader { text: qsTr("What runs in the background") }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("Nothing, once the app is closed. The system can store a volume for an output only while that output plays, so a value set for another one is kept by the app. While the app is open — in front or as a cover on the home screen — it listens to the sound system for that output to come into use and then sets the value. It listens only while a value waits. Closed, nothing of it runs; the value waits until the app is next open while that output plays.")
            }
            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: qsTr("Versions up to 0.2.1 used a service of the system for this (harbour-speakergain-apply), which kept running until the value was set, also across restarts of the phone. The update ends it, and the app removes what started it.")
            }

            SectionHeader { text: qsTr("What runs with system rights") }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("One helper, for USB headphones whose volume control reports a scale the system cannot use, such as the Bose Noise Cancelling Wired Earbuds: whatever is set, they stay at full volume. The package brings a sound profile that has the phone set their volume in software, and a list of the headphones known to need it. Headphones on that list need nothing more, and nothing runs with system rights for them.")
            }
            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("Other headphones with that fault are offered on the outputs page. Only when you switch one on there does the helper run, as the service harbour-speakergain-softvol@, for a few seconds. It does three things and ends: it writes the device's USB vendor and product id into /etc/udev/rules.d/91-harbour-speakergain-softvol-local.rules, has the system read its device rules again, and disconnects and reconnects that one USB device so the sound system takes it up with the new profile. Switching it off takes the id out again.")
            }
            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: qsTr("Starting a service of the system would normally ask for the device password. A rule the package brings (polkit) lets your user start exactly this one without it, and only as \"add\" or \"remove\" with a four-digit vendor and product id; the helper refuses anything else. It never starts by itself, never stays, and touches nothing but that one file and that one device. Removing the app removes the service, the rule and the list.")
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

            SectionHeader { text: qsTr("When something does not work") }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("If a slider moves but the sound does not change, a short report of how this phone's sound is put together helps to find out why. It holds no serial numbers, addresses or names, and you see all of it before you pass it on.")
            }
            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Diagnostics")
                onClicked: pageStack.push(Qt.resolvedUrl("DiagnosticsPage.qml"))
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
