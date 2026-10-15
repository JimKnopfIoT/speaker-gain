import QtQuick 2.0
import Sailfish.Silica 1.0

// The report is shown in full before it leaves the phone: whoever passes it
// on sees every line of it first.
Page {
    id: page
    allowedOrientations: Orientation.All

    property string report: ""

    function build() {
        report = speakergain.diagnosticReport()
    }

    Component.onCompleted: build()

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: col.height + Theme.paddingLarge

        PullDownMenu {
            MenuItem {
                text: qsTr("Save to Documents")
                onClicked: {
                    var path = speakergain.saveDiagnosticReport(page.report)
                    status.text = path.length > 0 ? qsTr("Saved as %1").arg(path)
                                                  : qsTr("Could not save the report.")
                }
            }
            MenuItem {
                text: qsTr("Copy to clipboard")
                onClicked: {
                    Clipboard.text = page.report
                    status.text = qsTr("Copied. Paste it into a message or an issue.")
                }
            }
            MenuItem {
                text: qsTr("Collect again")
                onClicked: { status.text = ""; page.build() }
            }
        }

        Column {
            id: col
            width: page.width
            spacing: Theme.paddingMedium

            PageHeader { title: qsTr("Diagnostics") }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("Plug in the headphones that misbehave and start some music before opening this page, then pull down to copy or save. The report describes how this phone's sound is wired up and names what looks wrong. It is not sent anywhere: you decide where it goes.")
            }
            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: qsTr("Left out or masked: serial numbers, Bluetooth addresses, the names given to Bluetooth devices, the phone's name and which apps are playing.")
            }
            Label {
                id: status
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.highlightColor
                visible: text.length > 0
            }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WrapAnywhere
                font.family: "monospace"
                font.pixelSize: Theme.fontSizeTiny
                color: Theme.secondaryColor
                text: page.report
            }
        }
        VerticalScrollDecorator {}
    }
}
