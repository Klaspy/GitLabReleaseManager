import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import CustomStyle
import qmlcomponents

import "./modal"
import "./components"
import "./stackelements"

ApplicationWindow {
    id: root_window

    width: 1280
    height: 720
    visible: true

    header: MenuBar {
        Menu {
            title: qsTr("File")

            MenuItem {
                text: qsTr("Private key list")
                onTriggered: pKeyList.open()
            }

            MenuItem {
                text: qsTr("Preferences")
                onTriggered: preferences.open()
            }
        }
    }

    PrivateKeysListDialog {id: pKeyList}

    PreferencesDialog {id: preferences}

    AddProjectDialog {id: addProjectDialog}

    background: Rectangle {
        color: "light gray"
    }

    Rectangle {
        anchors {
            top: parent.top
            left: parent.left
            right: separator.left
            bottom:parent.bottom
            margins: 10
        }

        radius: 10

        Column {
            id: projectsList_column
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            spacing: 10

            Text {
                id: projectsList_header
                font.pixelSize: 16
                font.bold: true
                text: qsTr("Projects  list")
                anchors.horizontalCenter: parent.horizontalCenter
                y: 5
                height: 30
                verticalAlignment: Text.AlignVCenter
            }

            Rectangle {
                id: projectsList_separator
                height: 1
                width: parent.width
                color: "black"
            }

            Button {
                text: "+"
                bgColor: "white"
                radius: 4

                anchors {
                    left: parent.left
                    right: parent.right
                    margins: 10
                }

                onClicked: {addProjectDialog.open()}
            }
        }

        Item {
            id: projectsList
            anchors {
                top: projectsList_column.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                topMargin: 10
                bottomMargin: 10
            }
            clip: true

            ButtonGroup {
                id: projectsList_group
                exclusive: true

                onCheckedButtonChanged: projectsList_lv.currentIndex = checkedButton.indx
            }

            ListView {
                id: projectsList_lv
                width: projectsList.width
                model: ProjectsListModel
                height: projectsList.height
                currentIndex: -1

                ScrollBar.vertical: ScrollBar {}

                delegate: Button {
                    ButtonGroup.group: projectsList_group
                    property var project: project_
                    text: project.id + ": " + project.name
                    checkable: true
                    property int indx: index
                    background: Rectangle {
                        color: checked ? "light blue" : "white"
                    }

                    contentItem: Text {
                        text: parent.text
                    }

                    width: projectsList.width
                    height: 30
                }
            }
        }
    }

    Item {
        id: separator
        width: 1
        x: parent.width / 4

        anchors {
            top: parent.top
            bottom:parent.bottom
        }

        MouseArea {
            cursorShape: Qt.SplitHCursor
            width: 10
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                bottom: parent.bottom
            }
            drag.target: separator
            drag.minimumX: root_window.width / 6
            drag.maximumX: root_window.width / 2
        }
    }

    Rectangle {
        id: content_rect
        anchors {
            top: parent.top
            left: separator.right
            right: parent.right
            bottom:parent.bottom
            margins: 10
        }

        width: parent.width / 4
        radius: 10
        visible: projectsList_lv.currentIndex > -1

        Column {
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            TabBar {
                id: stack_tb
                anchors {
                    left: parent.left
                    margins: 10
                }
                height: 40
                spacing: 0
                width: Math.min(contentWidth, parent.width - 20)
                clip: true

                Repeater {
                    id: stackHeader_repeater
                    model: [qsTr("Project"), qsTr("Releases"), qsTr("Tags")]

                    HeaderButton {
                        id: project_btn
                        text: modelData
                        font: projectsList_header.font
                        anchors.verticalCenter: parent.verticalCenter
                        rightBorder: index < stackHeader_repeater.count - 1
                        leftBorder: index > 0
                    }
                }
            }

            Rectangle {
                height: 1
                width: parent.width
                color: "black"
            }

            Item {width: 1; height: 10}

            StackLayout {
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: 10
                }
                height: content_rect.height - y - 10
                currentIndex: stack_tb.currentIndex

                ProjectStackElement {
                    project: ProjectsListModel.project(projectsList_lv.currentIndex)
                }
            }
        }
    }
}
