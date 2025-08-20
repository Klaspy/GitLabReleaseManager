import QtQuick
import QtQuick.Controls
import QtQuick.Effects

Button {
    id: control

    property int radius: 0
    property alias bgColor: bg.color
    property alias textColor: content.color
    font: GlobalStyle.controlFont

    onCheckableChanged: {
        checked = false
    }

    HoverHandler {
        cursorShape: control.hoverEnabled ? Qt.PointingHandCursor : Qt.ArrowCursor
    }

    contentItem: Text {
        id: content
        color: GlobalStyle.controlTextColor
        text: control.text
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font: control.font
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
