import Qml
import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Layouts

Button {
    id: control

    property int radius: 0
    property alias bgColor: bg.color
    property alias textColor: content.color
    font: GlobalStyle.controlFont

    HoverHandler {
        cursorShape: control.hoverEnabled ? Qt.PointingHandCursor : Qt.ArrowCursor
    }

    contentItem: GridLayout {
        id: contentGtid
        columnSpacing: control.display == Button.TextUnderIcon ? control.spacing : 0
        rowSpacing: control.display == Button.TextBesideIcon ? control.spacing : 0
        columns: control.display != Button.TextUnderIcon ? 2 : 1

        Image {
            id: icon
            source: control.icon.source
            Layout.preferredWidth: visible ? control.icon.width : 0
            Layout.preferredHeight: visible ? control.icon.height : 0
            fillMode: Image.PreserveAspectFit
            visible: control.display != Button.TextOnly && String(source) !== ""
            Layout.fillWidth: control.display == Button.IconOnly || control.display == Button.TextUnderIcon
            Layout.fillHeight: control.display == Button.IconOnly || control.display == Button.TextBesideIcon
        }

        Text {
            id: content
            color: GlobalStyle.controlTextColor
            text: control.text
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            Layout.fillWidth: true
            Layout.fillHeight: true
            font: control.font
            visible: control.display != Button.IconOnly
        }
    }

    background: Rectangle {
        id: bg
        color: GlobalStyle.controlBgColor
        anchors.fill: control
        radius: control.radius

        Behavior on color {ColorAnimation {duration: 100}}
        Behavior on opacity {PropertyAnimation {duration: 50}}

        RectangularShadow {
            anchors.fill: parent
            radius: parent.radius
            blur: 5
            spread: 2
            color: Qt.darker(parent.color)
            z: -1
        }

        Rectangle {
            id: blackout
            color: "black"
            anchors.fill: parent
            radius: control.radius
            opacity: 0

            Behavior on opacity {PropertyAnimation {duration: 50}}
        }
    }

    states: [
        State {
            name: "default"
            when: control.enabled && !control.pressed && !control.checked && !control.flat
            PropertyChanges {target: blackout; opacity: 0}
            PropertyChanges {target: bg; opacity: 1}
        },

        State {
            name: "clicked"
            when: control.enabled && (control.pressed || control.checked)
            PropertyChanges {target: blackout; opacity: 0.2}
        },

        State {
            name: "hovered"
            when: control.enabled && control.hoverEnabled && control.hovered
            PropertyChanges {target: blackout; opacity: 0.05}
        },

        State {
            name: "highlited"
            when: control.enabled && control.highlighted
        },

        State {
            name: "flat"
            when: control.enabled && control.flat
            PropertyChanges {target: bg; opacity: 0}
        },

        State {
            name: "disabled"
            when: !control.enabled
            PropertyChanges {target: bg; color: "light gray"}
        }
    ]
}
