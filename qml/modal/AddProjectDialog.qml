import QtQuick
import QtQuick.Controls
import QtQuick.Effects

import CustomStyle
import qmlcomponents

Dialog {
    id: dialog
    title: qsTr("Add new project")
    anchors.centerIn: parent
    width: 300
    height: bodyColumn.height + 80

    onClosed: {
        projectId_tf.text = ""
        pKey_cb.currentIndex = -1
        pKey_cb.displayText = ""
    }

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

    Column {
        id: bodyColumn
        anchors {
            left: parent.left
            right: parent.right
            margins: 10
        }
        spacing: 10

        TextField {
            id: projectId_tf
            background: Rectangle {
                color: "white"
                border.color: "light gray"
                border.width: 1
            }
            validator: IntValidator {bottom: 0}
            width: parent.width

            placeholderText: qsTr("Project id")
        }

        ComboBox {
            id: pKey_cb
            model: PKeyModel
            width: parent.width
            currentIndex: -1

            delegate: Text {
                text: display
                width: parent.width
                height: 30
                verticalAlignment: Text.AlignVCenter
                leftPadding: 10

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if (pKey_cb.currentIndex !== index)
                        {
                            pKey_cb.currentIndex = index
                            pKey_cb.displayText = text
                            pKey_cb.popup.close()
                        }
                    }
                }
            }
        }

        Text {
            id: errorText
            width: parent.width
            color: "red"
            font.pixelSize: 14
            height: 75
            wrapMode: Text.WordWrap

            function show(string) {
                text = string
                vanishing_anim.stop()
                errorText.opacity = 1
                errorText_vanishing.start()
            }

            PropertyAnimation {
                id: vanishing_anim
                target: errorText
                property: "opacity"
                from: 1
                to: 0
                duration: 2000
            }

            Timer {
                id: errorText_vanishing
                interval: 500
                onTriggered: vanishing_anim.start()
            }
        }

        Button {
            id: addProject_btn
            bgColor: "green"
            width: parent.width
            text: qsTr("Add project")
            enabled: pKey_cb.currentIndex > -1 && projectId_tf.text !== "" && !isAddingProject

            property bool isAddingProject: false

            onClicked: {
                let projectId = Number(projectId_tf.text)
                if (ProjectsListModel.containsProject(projectId)) {
                    errorText.show(qsTr("Project with current id already exists"))
                    return
                }

                isAddingProject = true
                ProjectsListModel.addProject(projectId, PKeyModel.getPKeyId(pKey_cb.currentIndex))
            }
        }

        Connections {
            target: ProjectsListModel

            function onProjectAdded() {
                addProject_btn.isAddingProject = false
                dialog.close()
            }

            function onAddProjectError(errorString) {
                errorText.show(errorString)
                addProject_btn.isAddingProject = false
            }
        }
    }
}
