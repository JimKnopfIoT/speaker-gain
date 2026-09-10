import QtQuick 2.0
import Sailfish.Silica 1.0

// Read only, on purpose. These tables decide what one press of the volume key
// does on each output, and they belong to a package: an update replaces them
// and takes any edit with it. So the app shows them and says so.
Page {
    id: page
    allowedOrientations: Orientation.All

    property var routes: []
    property var store: ({})

    Component.onCompleted: {
        page.routes = speakergain.routes()
        if (page.routes.length > 0)
            page.store = speakergain.storage(page.routes[0].path)
    }

    function curveLine(c) {
        if (c === undefined || c.count === undefined || c.count === 0)
            return qsTr("not listed")
        //: %1 step count, %2 quietest step in dB, %3 largest jump between two steps
        return qsTr("%1 steps  ·  down to %2 dB  ·  biggest jump %3 dB")
            .arg(c.count).arg(c.lowDb.toFixed(1)).arg(c.gapDb.toFixed(1))
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: col.height + Theme.paddingLarge

        Column {
            id: col
            width: page.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: qsTr("Volume steps")
                description: qsTr("What one key press does, per output")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: qsTr("The volume keys do not move a percentage, they walk through a list of fixed levels, and every output has a list of its own — one for calls, one for VoIP, one for everything else. A short list means coarse steps. The biggest jump is the largest gap between two neighbouring levels, which is what a single press costs you at its worst point.")
            }

            Repeater {
                model: page.routes
                delegate: Column {
                    width: page.width
                    spacing: Theme.paddingSmall

                    SectionHeader { text: modelData.name }

                    Label {
                        x: Theme.horizontalPageMargin
                        width: page.width - 2 * Theme.horizontalPageMargin
                        wrapMode: Text.Wrap
                        font.pixelSize: Theme.fontSizeExtraSmall
                        color: Theme.secondaryColor
                        visible: modelData.modes.length > 0
                        text: qsTr("Used by: %1").arg(modelData.modes.join(", "))
                    }

                    DetailItem {
                        label: qsTr("Calls")
                        value: page.curveLine(modelData.curves.call)
                    }
                    DetailItem {
                        label: qsTr("VoIP")
                        value: page.curveLine(modelData.curves.voip)
                    }
                    DetailItem {
                        label: qsTr("Everything else")
                        value: page.curveLine(modelData.curves.media)
                    }
                    DetailItem {
                        visible: modelData.highVolumeStep !== undefined
                        label: qsTr("Loudness warning above step")
                        value: modelData.highVolumeStep !== undefined ? modelData.highVolumeStep : ""
                    }
                }
            }

            SectionHeader { text: qsTr("Why there is no slider here") }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                text: page.store.headline !== undefined ? page.store.headline : ""
                color: page.store.verdict === "replaced" ? Theme.errorColor : Theme.highlightColor
            }
            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: (page.store.detail !== undefined ? page.store.detail + " " : "")
                      + qsTr("Changing these tables needs root, and the change would be gone again after the next update of the audio adaptation. This app therefore reads them and leaves them alone.")
            }
        }
        VerticalScrollDecorator {}
    }
}
