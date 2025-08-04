import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Layouts

import qmlcomponents
import CustomStyle

import "."

Dialog {
    id: dialog
    title: qsTr("Private key list")
    width: Math.max(200, tableView.childrenRect.width + 20)
    height: 400
    anchors.centerIn: parent

    header: Item {
        height: 50
        Text {
            anchors.centerIn: parent
            text: dialog.title
        }
        MenuSeparator {
            anchors.bottom: parent.bottom
            width: parent.width
        }
    }

    background: Rectangle {
        radius: 10
        border.width: 1

        RectangularShadow {
            anchors.fill: parent
            radius: parent.radius
            blur: 30
            spread: 5
            color: Qt.darker(parent.color)
            z: -1
        }
    }

    HorizontalHeaderView {
        id: tableHeader
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            rightMargin: 4
        }
        syncView: tableView

        delegate: Rectangle {
            border.color: "black"
            color: "light gray"
            implicitHeight: 30
            implicitWidth: 100

            Text {
                text: display
                anchors.centerIn: parent
            }
        }
    }

    ScrollView {
        id: sv
        anchors {
            top: tableHeader.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            rightMargin: -4
        }
        clip: true

        TableView {
            id: tableView
            model: PKeyModel

            delegate: Rectangle {
                implicitHeight: 20
                implicitWidth: Math.max(text.contentWidth + 10, tableView.width / 2)
                border.width: 1

                TextInput {
                    id: text
                    anchors.centerIn: parent
                    text: display
                    font: GlobalStyle.appFont
                    readOnly: true
                    echoMode: column == 1 ? TextInput.Password : TextInput.Normal
                }

                MouseArea {
                    acceptedButtons: Qt.RightButton
                    anchors.fill: parent
                    onClicked: {
                        deleteMenu.x = mouseX
                        deleteMenu.y = mouseY
                        deleteMenu.open()
                    }

                    Menu {
                        id: deleteMenu

                        MenuItem {
                            text: qsTr("copy key")
                            onTriggered: PKeyModel.copyKey(row)
                        }

                        MenuItem {
                            text: qsTr("delete key")
                            onTriggered: PKeyModel.removeKey(row)
                        }
                    }
                }

                MouseArea {
                    anchors.fill: column == 1 ? parent : undefined
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        if (text.echoMode == TextInput.Password) {
                            text.echoMode  = TextInput.Normal
                        }
                        else {
                            text.echoMode  = TextInput.Password
                        }
                    }
                }
            }
        }
    }

    Dialog {
        id: addDialog
        height: 200
        width: 250
        x: -11
        anchors.centerIn: parent

        Column {
            width: parent.width
            spacing: 20

            TextField {
                id: keyName_field
                placeholderText: qsTr("Key name")
                width: parent.width
            }

            TextField {
                id: key_field
                placeholderText: qsTr("Key")
                width: parent.width
                height: keyName_field.height
                echoMode: TextInput.Password

                ToolTip {
                    id: keyError_popup
                    anchors.centerIn: parent
                    timeout: 1000

                    enter: Transition {
                        NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 80}
                    }

                    exit: Transition {
                        NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 80}
                    }


                    background: Rectangle {
                        opacity: 70
                        color: "gray"
                        radius: 10

                        RectangularShadow {
                            anchors.fill: parent
                            radius: parent.radius
                            blur: 5
                            spread: 2
                            color: Qt.darker(parent.color)
                            z: -1
                        }
                    }
                }

                background: Rectangle {
                    color: "white"
                    border.color: keyError_popup.opened ? "red" : "light gray"
                    border.width: 1
                }
            }
        }

        background: Rectangle {
            radius: 10
            border.width: 1

            RectangularShadow {
                anchors.fill: parent
                radius: parent.radius
                blur: 30
                spread: 5
                color: Qt.darker(parent.color)
                z: -1
            }
        }

        footer: Item {
            height: 50

            RowLayout {
                spacing: 10
                anchors.fill: parent
                anchors.margins: 10

                Button {
                    text: qsTr("Save")
                    bgColor: "green"
                    Layout.fillWidth: true

                    onClicked: {
                        if (key_field.text === "") {
                            keyError_popup.text = qsTr("Field \"key\" is empty")
                            keyError_popup.open()
                            return
                        }
                        if (!PKeyModel.canAddKey(key_field.text)) {
                            keyError_popup.text = qsTr("Key already exist")
                            keyError_popup.open()
                            return
                        }

                        addDialog.accept()
                    }
                }

                Button {
                    text: qsTr("Cancel")
                    bgColor: "white"
                    Layout.fillWidth: true


                    onClicked: {
                        addDialog.reject()
                    }
                }
            }
        }

        onAccepted: {
            PKeyModel.addKey(keyName_field.text, key_field.text)
        }
    }

    footer: Item {
        height: 50

            Button {
                text: qsTr("+")
                bgColor: "white"
                radius: 5
                font.pixelSize: 20
                anchors.fill: parent
                anchors.margins: 10

                onClicked: addDialog.open()
            }
        }
}
