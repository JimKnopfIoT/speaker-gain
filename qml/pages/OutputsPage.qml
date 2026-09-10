import QtQuick 2.0
import Sailfish.Silica 1.0

// The volume PulseAudio keeps per role and per output route. Only the route
// the device is playing on can be written — the value is stored under the
// name of the route in use, so the headphones have to be connected to set
// theirs.
Page {
    id: page
    allowedOrientations: Orientation.All

    property var entries: []
    property string route: ""
    property string statusText: ""
    property bool statusGood: true

    function reload() {
        page.route = speakergain.activeRoute()
        page.entries = speakergain.outputVolumes()
    }

    function apply(role, v) {
        var ok = speakergain.setRoleVolume(role, v)
        page.statusGood = ok
        page.statusText = ok
            ? qsTr("Set to %1 %. The database behind it is written a few seconds later.").arg(v)
            : (speakergain.lastError.length > 0 ? speakergain.lastError : qsTr("Could not set the value."))
    }

    Component.onCompleted: page.reload()

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: col.height + Theme.paddingLarge

        PullDownMenu {
            MenuItem { text: qsTr("Read again"); onClicked: page.reload() }
        }

        Column {
            id: col
            width: page.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: qsTr("Per output")
                description: page.route.length > 0
                             ? qsTr("Playing on: %1").arg(page.route)
                             : qsTr("No output could be identified")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: qsTr("Speaker, wired and Bluetooth each carry a volume of their own, per kind of sound. The system stores them in your home directory, one entry per pairing of the two, and restores them when the output changes. Only the output in use can be set here: the value is filed under its name, so the headphones have to be connected to give them one.")
            }

            SectionHeader { text: qsTr("The output in use") }

            Repeater {
                model: page.entries
                delegate: Column {
                    width: page.width
                    visible: modelData.current === true

                    Slider {
                        width: page.width
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        value: modelData.percent
                        valueText: Math.round(value) + " %"
                        label: modelData.roleLabel + "  ·  " + modelData.routeLabel
                        onReleased: page.apply(modelData.role, Math.round(value))
                    }
                }
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
                visible: page.route === "bta2dp" || page.route === "btmono"
                text: qsTr("On Bluetooth the phone takes over the headphones' own volume control and holds it at full scale, so everything you hear is set by the value above alone. Lowering it is the only way down — and if the headphones have a volume of their own, it stays where the phone put it after disconnecting.")
            }

            SectionHeader {
                text: qsTr("What the headphones themselves decide")
                visible: page.route === "bta2dp" || page.route === "btmono"
            }
            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                visible: page.route === "bta2dp" || page.route === "btmono"
                text: qsTr("Bluetooth headphones carry settings no audio system can reach, because they live in the headphones and not in the phone: the listening modes — noise cancelling and ambient sound — and the charge of their battery. For Sony models there is a client for Sailfish OS, called Lauscher, that speaks their own protocol and offers exactly those. It is worth knowing about here because ambient sound changes how loud the world around you is, which is half of what people mean by too loud. Setting a volume is not among its functions at the time of writing, so the slider above stays the only way to make the music itself quieter.")
            }

            SectionHeader { text: qsTr("Every stored value") }

            Repeater {
                model: page.entries
                delegate: DetailItem {
                    label: modelData.route.length > 0
                           ? modelData.roleLabel + " · " + modelData.routeLabel
                           : modelData.roleLabel
                    value: modelData.percent.toFixed(1) + " %"
                           + (modelData.current ? "  ·  " + qsTr("in use") : "")
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WrapAnywhere
                font.pixelSize: Theme.fontSizeTiny
                font.family: "monospace"
                color: Theme.secondaryColor
                text: page.entries.length > 0 ? page.entries[0].source : ""
            }
        }
        VerticalScrollDecorator {}
    }
}
