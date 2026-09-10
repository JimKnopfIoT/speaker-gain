import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {
    Column {
        anchors.centerIn: parent
        width: parent.width - 2 * Theme.paddingLarge
        spacing: Theme.paddingSmall

        Label {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("Alarm")
            color: Theme.secondaryColor
            font.pixelSize: Theme.fontSizeSmall
        }
        Label {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            text: speakergain.alarmVolume >= 0 ? speakergain.alarmVolume + " %" : "—"
            font.pixelSize: Theme.fontSizeHuge
        }
    }
}
