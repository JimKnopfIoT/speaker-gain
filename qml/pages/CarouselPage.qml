import QtQuick 2.0
import Sailfish.Silica 1.0

// The three views side by side, in a ring: the volume steps on the left, the
// alarm in the middle, the sliders per output on the right. A swipe past
// either end comes round to the other, so every view is one swipe away from
// both of the others.
Page {
    id: page
    allowedOrientations: Orientation.All

    SlideshowView {
        id: slides
        anchors.fill: parent
        itemWidth: width
        itemHeight: height
        currentIndex: 1

        model: VisualItemModel {
            StepsView {
                width: slides.width
                height: slides.height
                current: slides.currentIndex === 0
            }
            AlarmView {
                width: slides.width
                height: slides.height
                current: slides.currentIndex === 1
            }
            OutputsView {
                width: slides.width
                height: slides.height
                current: slides.currentIndex === 2
            }
        }
    }

    // Where in the ring you are.
    Row {
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: Theme.paddingMedium
        }
        spacing: Theme.paddingMedium

        Repeater {
            model: slides.count
            Rectangle {
                width: Theme.paddingSmall * 2
                height: width
                radius: width / 2
                color: index === slides.currentIndex ? Theme.highlightColor : Theme.secondaryColor
                opacity: index === slides.currentIndex ? 1.0 : 0.4
            }
        }
    }
}
