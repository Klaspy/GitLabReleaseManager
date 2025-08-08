import QtQuick
import QtQuick.Controls

import CustomStyle

TabButton {
    property bool leftBorder
    property bool rightBorder

    width: contentItem.contentWidth + 20

    contentItem: Text {
        color: "black"
        text: parent.text
        font: parent.font
        horizontalAlignment: Text.AlignHCenter
    }

    Rectangle {
        color: "gray"
        height: parent.height
        width: 0.5
        visible: leftBorder
    }

    Rectangle {
        color: "gray"
        height: parent.height
        width: 0.5
        x: parent.width - 0.5
        visible: rightBorder
    }

    background: Rectangle {
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 4
        }
        width: parent.width - 10
        height: 1
        color: "gray"
        visible: parent.checked
    }
}
