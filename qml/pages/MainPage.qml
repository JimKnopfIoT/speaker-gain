import QtQuick 2.0
import Sailfish.Silica 1.0

// The one knob that works today: the alarm volume, written into profiled's
// general profile. Everything else on this page explains why the knob has to
// exist at all, and whether the value will still be there tomorrow.
Page {
    id: page
    allowedOrientations: Orientation.All

    property string statusText: ""
    property bool statusGood: true
    property var store: ({})

    function apply(v) {
        var ok = speakergain.setAlarmVolume(v)
        page.statusGood = ok
        page.statusText = ok
            ? qsTr("Set to %1 % and read back.").arg(v)
            : (speakergain.lastError.length > 0 ? speakergain.lastError
                                                : qsTr("Could not set the value."))
    }

    Component.onCompleted: page.store = speakergain.alarmStorage()

    onStatusChanged: if (status === PageStatus.Activating) speakergain.refresh()

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: col.height + Theme.paddingLarge

        PullDownMenu {
            MenuItem {
                text: qsTr("About")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }
            MenuItem {
                text: qsTr("Volume steps per output")
                onClicked: pageStack.push(Qt.resolvedUrl("RoutesPage.qml"))
            }
        }

        Column {
            id: col
            width: page.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: qsTr("Speaker Gain")
                description: qsTr("The output volumes the system keeps to itself")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.highlightColor
                visible: !speakergain.available
                text: qsTr("profiled is not answering. Without it this page cannot read or write anything.")
            }

            SectionHeader { text: qsTr("Alarm") }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: qsTr("The alarm has a loudness of its own. It is not the ringtone volume, the volume keys do not reach it, and switching to Silent leaves it untouched — by design, because an alarm is meant to wake you. What is missing is a way to set it, and this is that way.")
            }

            Slider {
                id: alarmSlider
                width: page.width
                enabled: speakergain.available && speakergain.alarmVolume >= 0
                minimumValue: 0
                maximumValue: 100
                stepSize: 5
                value: speakergain.alarmVolume >= 0 ? speakergain.alarmVolume : 100
                valueText: Math.round(value) + " %"
                label: qsTr("Alarm volume")
                onReleased: page.apply(Math.round(value))
            }

            DetailItem {
                label: qsTr("Shipped default")
                value: {
                    var d = speakergain.defaultValue("clock.alert.volume")
                    return d >= 0 ? d + " %" : qsTr("unknown")
                }
            }
            DetailItem {
                label: qsTr("Ringtone, for comparison")
                value: speakergain.ringVolume >= 0 ? speakergain.ringVolume + " %" : qsTr("unknown")
            }
            DetailItem {
                label: qsTr("Active profile")
                value: speakergain.currentProfile.length > 0 ? speakergain.currentProfile : qsTr("unknown")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: page.statusGood ? Theme.secondaryHighlightColor : Theme.errorColor
                visible: page.statusText.length > 0
                text: page.statusText
            }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: qsTr("Every alarm fades in over its first 20 seconds, from silence up to the value above. So this is the ceiling of that ramp, not the volume it starts at.")
            }

            ButtonLayout {
                Button {
                    text: qsTr("Back to the default")
                    enabled: speakergain.available && speakergain.defaultValue("clock.alert.volume") >= 0
                    onClicked: page.apply(speakergain.defaultValue("clock.alert.volume"))
                }
            }

            SectionHeader { text: qsTr("Does this value survive?") }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                text: page.store.headline !== undefined ? page.store.headline : qsTr("unknown")
                color: page.store.verdict === "replaced" ? Theme.errorColor : Theme.highlightColor
            }
            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: page.store.detail !== undefined ? page.store.detail : ""
            }
            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeTiny
                font.family: "monospace"
                color: Theme.secondaryColor
                text: page.store.path !== undefined ? page.store.path : ""
            }
            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: qsTr("Written straight away, but the file behind it is only flushed a few seconds later. If the phone is cut from power right after a change, the change is the thing that is lost.")
            }
        }
        VerticalScrollDecorator {}
    }
}
