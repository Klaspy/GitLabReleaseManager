import QtQuick
import QtQuick.Controls
import QtQuick.Shapes
import QtQuick.Dialogs as Dialogs
import QtQuick.Layouts

import CustomStyle
import qmlcomponents

import "../components"

Dialog {
    id: dialog
    required property Project project

    header: TextArea {
        id: title_ta
        font.bold: true
        font.pixelSize: 16
        text: tag_cb.currentIndex < 0 ? qsTr("New Release") : qsTr("Release ") + tag_cb.currentText

        background: Rectangle {
            color: "gray"
            height: 1
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                leftMargin: 5
                rightMargin: 5
            }
        }
    }
    width: parent.width
    height: parent.height

    onClosed: {
        tabBar.currentIndex = 0
        tag_cb.currentIndex = -1
        editDesc_ta.text = ""
    }

    ScrollView {
        id: body_sv
        anchors.fill: parent
        anchors.topMargin: -10
        anchors.bottomMargin: -10

        Column {
            width: body_sv.width - 30
            x: 15
            spacing: 10

            Item {
                width: 1
                height: 10
            }

            Row {
                spacing: 10

                Text {
                    text: qsTr("Tag")
                    anchors.verticalCenter: parent.verticalCenter
                }

                ComboBox {
                    id: tag_cb
                    model: project == null ? undefined : project.tags
                    textRole: "name"
                    enabledRole: "canCreateRelease"

                    onModelChanged: currentIndex = -1
                }

                Button {
                    bgColor: "white"
                    height: tag_cb.height
                    width: tag_cb.height
                    radius: tag_cb.height
                    icon.source: "qrc:/images/reload.png"
                    display: Button.IconOnly
                    onClicked: project.updateTags()
                }
            }

            Text {
                height: 40
                text: qsTr("Description")
                font.pixelSize: 14
                font.bold: true
                verticalAlignment: Text.AlignBottom
            }

            Rectangle {
                id: descriptionRect
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: 5
                }
                layer.enabled: true
                clip: true

                height: 256
                border.width: 1
                radius: 10

                TabBar {
                    id: tabBar
                    anchors {
                        left: parent.left
                        margins: 10
                    }
                    height: 40
                    spacing: 0
                    width: Math.min(contentWidth, parent.width - 20)
                    clip: true

                    background: Item{}

                    Repeater {
                        id: stackHeader_repeater
                        model: [qsTr("Edit"), qsTr("Preview")]

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
                    id: desc_delimiter
                    color: "gray"
                    width: parent.width
                    height: 1
                    anchors.top: tabBar.bottom
                }

                ScrollView {
                    id: desc_sv
                    anchors {
                        top: desc_delimiter.bottom
                        left: parent.left
                        right: parent.right
                        bottom: parent.bottom
                    }

                    TextArea {
                        id: editDesc_ta
                        background: Item{}
                        wrapMode: TextEdit.WordWrap
                        height: Math.max(desc_sv.height, contentHeight)
                        visible: tabBar.currentIndex === 0
                        textFormat: TextEdit.PlainText
                        onVisibleChanged: if (!visible) focus = false
                    }

                    TextArea {
                        background: Item{}
                        wrapMode: TextEdit.WordWrap
                        height: Math.max(desc_sv.height, contentHeight)
                        visible: tabBar.currentIndex === 1
                        readOnly: true
                        textFormat: TextEdit.MarkdownText
                        text: editDesc_ta.text

                        MouseArea {anchors.fill: parent; acceptedButtons: Qt.NoButton}
                    }
                }
            }


            Text {
                height: 40
                text: qsTr("Release files")
                font.pixelSize: 14
                font.bold: true
                verticalAlignment: Text.AlignBottom
            }

            Repeater {
                id: releaseLinks_repeater
                width: parent.width
                property list<ReleaseLink> releaseLinks
                model: releaseLinks

                function syncModel() {
                    for (let i = 0; i < count; ++i) {
                        itemAt(i).syncModel()
                    }
                }

                RowLayout {
                    id: releaseRow
                    width: parent.width
                    required property string name
                    required property string url
                    required property int type
                    required property int index
                    spacing: 10

                    function syncModel() {
                        releaseLinks_repeater.model[index].name = name
                    }

                    TextField {
                        id: fileName
                        text: parent.name
                        validator: RegularExpressionValidator {
                            regularExpression: /[A-Za-z0-9-._~:#!$'\\(\\)\\*\\+,;=]*/
                        }

                        background: Rectangle {border.width: 1}

                        onEditingFinished: parent.name = text
                    }

                    Item {
                        clip: true
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Text {
                            id: urlText
                            text: releaseRow.url
                            Layout.fillWidth: true
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        MouseArea {
                            drag {
                                target: urlText
                                minimumX: Math.min(parent.width - urlText.width, 0)
                                maximumX: 0
                                axis: Drag.XAxis
                            }
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                        }
                    }

                    Button {
                        bgColor: "white"
                        text: "X"
                        textColor: "red"
                        font.family: "Arial"
                        onClicked: {
                            releaseLinks_repeater.model.splice(parent.index, 1)
                        }
                    }
                }
            }

            Button {
                text: "+"
                bgColor: "white"
                width: parent.width
                onClicked: fileDialog.open()

                Dialogs.FileDialog {
                    id: fileDialog
                    fileMode: Dialogs.FileDialog.OpenFile
                    nameFilters: [qsTr("All files (*)")]

                    onAccepted: {
                        var newLink = new ReleaseLink()
                        newLink.name = String(selectedFile).split("/").pop()
                        newLink.url = String(selectedFile)
                        newLink.type = ReleaseLink.Package

                        releaseLinks_repeater.model.push(newLink)
                    }
                }
            }

            Item {
                width: 1
                height: 10
            }
        }
    }

    footer: Rectangle {
        height: 50
        border.width: 1

        Row {
            width: parent.width
            height: parent.height
            layoutDirection: Qt.RightToLeft
            rightPadding: 10
            spacing: 10

            Button {
                text: qsTr("Cancel")
                bgColor: "white"
                anchors.verticalCenter: parent.verticalCenter
                radius: 4

                onClicked: dialog.reject()
            }

            Button {
                text: qsTr("Save")
                bgColor: "dodgerblue"
                anchors.verticalCenter: parent.verticalCenter
                enabled: dialog.visible && tag_cb.currentIndex !== -1 && title_ta.text !== ""
                radius: 4

                onClicked: {
                    releaseLinks_repeater.syncModel()
                    console.log(releaseLinks_repeater.model)
                    if (project.createRelease(tag_cb.currentText, title_ta.text, editDesc_ta.text,
                                              releaseLinks_repeater.model))
                    {
                        dialog.accept();
                    }
                }
            }
        }
    }
}
