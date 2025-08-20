import QtQuick 2.15

import qmlcomponents 1.0
import CustomStyle

Item {
    id: elementRoot
    clip: true

    property Project project

    Loader {
        sourceComponent: project !== null ? body_component : undefined
    }

    Component {
        id: body_component

        Column {
            width: parent.width
            spacing: 10

            Text {
                font.pixelSize: 30
                font.bold: true

                text: project.name
            }

            Text {text: "Id: " + project.id}

            Text {text: "Create date time: " + project.createDT}

            Row {
                property UserData author: project.author

                Text {text: qsTr("Author") + ": "}

                Text {
                    text: "<a href=\"1.1\">" + parent.author.name + "</a>"

                    onLinkActivated: Qt.openUrlExternally(parent.author.gitUrl)

                    MouseArea {
                        acceptedButtons: Qt.NoButton; cursorShape: Qt.PointingHandCursor; anchors.fill: parent
                    }
                }
            }

            Text {
                text: "<a href=\"1.1\">" + qsTr("Open in browser") + "</a>"

                onLinkActivated: Qt.openUrlExternally(project.url)

                MouseArea {
                    acceptedButtons: Qt.NoButton; cursorShape: Qt.PointingHandCursor; anchors.fill: parent
                }
            }
        }
    }
}
