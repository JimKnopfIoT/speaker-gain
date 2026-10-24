import QtQuick 2.0
import Sailfish.Silica 1.0

// The volume PulseAudio keeps per role and per output route, for every route
// the system knows. The route in use takes a value at once; PulseAudio files
// no other, so for those the value waits and the app applies it when the
// route comes into use while it runs (SpeakerGain::watchPending).
// Right of the alarm in the carousel.
Item {
    id: page

    property bool current: false

    property var entries: []
    property var routeList: []
    property var sinks: []
    property bool pendingHere: false
    property string route: ""
    property string statusText: ""
    property bool statusGood: true

    function reload() {
        page.route = speakergain.activeRoute()
        page.entries = speakergain.outputVolumes()
        page.routeList = speakergain.allRoutes()
        page.sinks = speakergain.deviceSinks()
        var waiting = false
        for (var i = 0; i < page.routeList.length; ++i) {
            var r = page.routeList[i]
            if (!r.current)
                continue
            for (var j = 0; j < r.entries.length; ++j)
                if (r.entries[j].pending >= 0)
                    waiting = true
        }
        page.pendingHere = waiting
    }

    function apply(route, current, role, v) {
        var ok = speakergain.setRouteVolume(route, role, v)
        page.statusGood = ok
        page.statusText = !ok
            ? (speakergain.lastError.length > 0 ? speakergain.lastError : qsTr("Could not set the value."))
            : current
              ? qsTr("Set to %1 %. The database behind it is written a few seconds later.").arg(v)
              : qsTr("Kept at %1 %. It is set once this output is in use while the app is open.").arg(v)
        page.routeList = speakergain.allRoutes()
    }

    function forget(route, role) {
        speakergain.clearPending(route, role)
        page.routeList = speakergain.allRoutes()
    }

    function applySink(name, v) {
        var ok = speakergain.setSinkVolume(name, v)
        page.statusGood = ok
        page.statusText = ok
            ? qsTr("Set to %1 %. It holds while the output stays connected.").arg(v)
            : (speakergain.lastError.length > 0 ? speakergain.lastError : qsTr("Could not set the value."))
        page.sinks = speakergain.deviceSinks()
    }

    // USB headphones whose volume scale PulseAudio cannot use: the root
    // helper lists them for software volume and plugs them in anew. Takes a
    // few seconds; the page reads everything again afterwards.
    function softVolume(usbId, on) {
        var ok = speakergain.setSoftVolume(usbId, on)
        page.statusGood = ok
        page.statusText = ok
            ? (on ? qsTr("The volume of these headphones is now set in software. They were connected anew for it.")
                  : qsTr("Back to the device's own volume control. The headphones were connected anew for it."))
            : (speakergain.lastError.length > 0 ? speakergain.lastError : qsTr("Could not set the value."))
        softVolumeReload.start()
    }

    // The re-enumerated device reappears in PulseAudio a moment after the
    // helper returns.
    Timer {
        id: softVolumeReload
        interval: 2500
        onTriggered: page.reload()
    }

    Component.onCompleted: page.reload()

    // The output can change while this page is open or lying behind another
    // one: headphones are unplugged, a headset disconnects, a cable goes in.
    // Reading once at open leaves the page offering a route that is no longer
    // in use, and a slider moved there writes the value under whichever route
    // the stream actually lands on. So it re-reads whenever it comes forward,
    // and on a slow beat while it is in front.
    onCurrentChanged: if (current) page.reload()

    Timer {
        interval: 4000
        repeat: true
        running: page.current && Qt.application.active
        onTriggered: {
            // Also while a value waits for the route in use: the app
            // applies it a moment after the switch, and the page should show
            // it landing.
            var now = speakergain.activeRoute()
            if (now !== page.route || page.pendingHere)
                page.reload()
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: col.height + 3 * Theme.paddingLarge

        PullDownMenu {
            MenuItem {
                text: qsTr("About")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }
        }

        Column {
            id: col
            width: page.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: qsTr("Per output")
                description: page.route.length > 0
                             ? qsTr("Playing on: %1").arg(speakergain.routeName(page.route))
                             : qsTr("No output could be identified")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: qsTr("Every output carries a volume of its own, per kind of sound, and the system restores it when the output changes. The output in use takes a new value at once. The system files no other, so for those the value is kept here and set once that output is in use while the app is open, in front or as a cover. Nothing of it runs once the app is closed.")
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

            Repeater {
                model: page.routeList
                delegate: Column {
                    id: routeBlock
                    width: page.width
                    property var routeData: modelData

                    SectionHeader {
                        text: routeBlock.routeData.current
                              ? routeBlock.routeData.label + "  ·  " + qsTr("in use")
                              : routeBlock.routeData.label
                    }

                    Repeater {
                        model: routeBlock.routeData.entries
                        delegate: Column {
                            width: page.width

                            Slider {
                                width: page.width
                                minimumValue: 0
                                maximumValue: 100
                                stepSize: 1
                                value: modelData.pending >= 0 ? modelData.pending : modelData.percent
                                valueText: Math.round(value) + " %"
                                label: modelData.roleLabel
                                onReleased: page.apply(routeBlock.routeData.route,
                                                       routeBlock.routeData.current,
                                                       modelData.role, Math.round(value))
                            }
                            Label {
                                x: Theme.horizontalPageMargin
                                width: page.width - 2 * Theme.horizontalPageMargin
                                wrapMode: Text.Wrap
                                font.pixelSize: Theme.fontSizeExtraSmall
                                color: modelData.pending >= 0 ? Theme.secondaryHighlightColor
                                                              : Theme.secondaryColor
                                visible: modelData.pending >= 0 || modelData.stored === false
                                text: modelData.pending >= 0
                                      ? qsTr("Waiting: %1 % once this output is in use while the app is open.").arg(modelData.pending)
                                      : qsTr("Nothing stored yet; this is the figure the system starts this output from.")
                            }
                            Button {
                                x: Theme.horizontalPageMargin
                                visible: modelData.pending >= 0
                                text: qsTr("Forget")
                                onClicked: page.forget(routeBlock.routeData.route, modelData.role)
                            }
                        }
                    }
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                visible: page.route.indexOf("bt") === 0
                text: qsTr("On Bluetooth the phone takes over the headphones' own volume control and holds it at full scale, so what you hear is set by the value above. The headphones' own volume further down can be lowered as well — it stays where the phone left it after disconnecting.")
            }

            SectionHeader {
                text: qsTr("The output's own volume")
                visible: page.sinks.length > 0
            }
            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                visible: page.sinks.length > 0
                text: qsTr("Beneath the values above, Bluetooth headphones and USB sound cards have a volume of their own, and it starts at full scale. On Bluetooth the figure is sent to the headphones and becomes their own volume; on USB it is the card's mixer. The system stores none of these: a value holds while the output stays connected, and the next connection may start at full scale again.")
            }
            Repeater {
                model: page.sinks
                delegate: Column {
                    width: page.width
                    spacing: Theme.paddingSmall

                    // Broken scale and nothing done about it yet.
                    property bool needsSoft: modelData.scaleBroken === true && modelData.softVolume !== true

                    Slider {
                        width: page.width
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        value: modelData.percent
                        valueText: Math.round(value) + " %"
                        label: modelData.label
                               + (modelData.kind === "bluetooth" ? "  ·  " + qsTr("Bluetooth")
                                  : modelData.kind === "usb" ? "  ·  " + qsTr("USB") : "")
                        onReleased: page.applySink(modelData.name, Math.round(value))
                    }
                    Label {
                        x: Theme.horizontalPageMargin
                        width: page.width - 2 * Theme.horizontalPageMargin
                        wrapMode: Text.Wrap
                        font.pixelSize: Theme.fontSizeExtraSmall
                        color: Theme.highlightColor
                        visible: parent.needsSoft
                        text: qsTr("These headphones report a volume scale the system cannot use: whatever is set, they stay at full volume. Speaker Gain can have the phone set their volume in software instead. They are connected anew for it, which takes a few seconds.")
                    }
                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        visible: parent.needsSoft
                        text: qsTr("Set volume in software")
                        onClicked: page.softVolume(modelData.usbId, true)
                    }
                    Label {
                        x: Theme.horizontalPageMargin
                        width: page.width - 2 * Theme.horizontalPageMargin
                        wrapMode: Text.Wrap
                        font.pixelSize: Theme.fontSizeExtraSmall
                        color: Theme.secondaryColor
                        visible: modelData.scaleBroken === true && modelData.softVolume === true
                        text: qsTr("The volume of these headphones is set in software, because their own volume control reports a scale the system cannot use.")
                    }
                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        visible: modelData.softVolumeSource === "local"
                        text: qsTr("Use the device's own control again")
                        onClicked: page.softVolume(modelData.usbId, false)
                    }
                }
            }

            SectionHeader {
                text: qsTr("What the headphones themselves decide")
                visible: page.route.indexOf("bt") === 0
            }
            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                visible: page.route.indexOf("bt") === 0
                text: qsTr("Bluetooth headphones carry settings no audio system can reach, because they live in the headphones and not in the phone: the listening modes — noise cancelling and ambient sound — and the charge of their battery. For Sony models there is a client for Sailfish OS, called Lauscher, that speaks their own protocol and offers exactly those. It is worth knowing about here because ambient sound changes how loud the world around you is, which is half of what people mean by too loud. Setting a volume is not among its functions at the time of writing, so the slider above stays the only way to make the music itself quieter.")
            }

            SectionHeader { text: qsTr("Fixed volumes of the policy") }

            Repeater {
                model: page.entries
                delegate: DetailItem {
                    visible: modelData.route.length === 0
                    label: modelData.route.length > 0
                           ? modelData.roleLabel + " · " + modelData.routeLabel
                           : modelData.roleLabel
                    value: modelData.percent.toFixed(1) + " %"
                           + (modelData.current ? "  ·  " + qsTr("in use") : "")
                           + (modelData.stored === false ? "  ·  " + qsTr("not stored yet") : "")
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
