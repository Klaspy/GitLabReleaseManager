import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Shapes

import qmlcomponents
import CustomStyle

Column {
    id: release_col
    width: releases_root.width
    spacing: 0
    property ReleaseData release: release_
    property int accessLevel
    height: 30 + release_bodyShape.height

    Shape {
        width: parent.width
        height: 30
        
        ShapePath {
            fillColor: "#f0f0f0"
            strokeWidth: 1
            strokeColor: "black"
            startX: 11
            startY: 1
            
            PathLine {x: width - 10; y: 1}
            PathArc {radiusX: 10; radiusY: 10; relativeX: 10; y: 11}
            PathLine {relativeX: 0; y: 30}
            PathLine {x: 1; relativeY: 0}
            PathLine {relativeX: 0; y: 11}
            PathArc {radiusX: 10; radiusY: 10; x: 10; y: 1}
        }
        
        Row {
            height: parent.height
            x: 10
            spacing: 5

            Text {
                id: expand_btn
                text: "➤"
                property bool expanded: false
                rotation: expanded ? 90 : 270
                anchors.verticalCenter: parent.verticalCenter

                Behavior on rotation {
                    NumberAnimation {duration: 200}
                }
            }
            
            Text {
                text: release.name
                font.bold: true
                font.pixelSize: 16
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 2
            }
            
            Text {
                color: "#505050"
                text: qsTr("released at") + " " + release.releaseDTString()
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 2
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: expand_btn.expanded = !expand_btn.expanded
            cursorShape: Qt.PointingHandCursor
        }
    }
    
    Shape {
        id: release_bodyShape
        width: parent.width
        height: expand_btn.expanded ? body_column.height + 10 : 15
        clip: true
        Behavior on height {
            NumberAnimation {duration: 200}
        }
        
        ShapePath {
            strokeWidth: 1
            strokeColor: "black"
            startX: 1
            startY: 0
            
            PathLine {x: width; y: 0}
            PathLine {relativeX: 0; y: release_bodyShape.height - 10}
            PathArc {radiusX: 10; radiusY: 10; relativeX: -10; y: release_bodyShape.height}
            PathLine {x: 11; y: release_bodyShape.height}
            PathArc {radiusX: 10; radiusY: 10; relativeX: -10; y: release_bodyShape.height - 10}
            PathLine {x: 1; y: 0}
        }

        Column {
            id: body_column
            anchors {
                left: parent.left
                right: parent.right
                margins: 10
            }

            Text {
                text: qsTr("Description") + ":"
                font.bold: true
                font.pixelSize: 16
            }

            TextArea {
                readOnly: true
                text: release.description
                textFormat: TextEdit.MarkdownText
                font.pixelSize: 14
            }

            Text {
                text: qsTr("Assets") + ":"
                font.bold: true
                font.pixelSize: 16
                visible: sources_repeater.count > 0
            }

            Repeater {
                id: sources_repeater
                model: release.sourceCodes

                delegate: Text {
                    property ReleaseLink releaseLink: sources_repeater.model[index]
                    x: 10
                    text: "* <a href=\"1.1\">" + qsTr("source codes") + " (" + releaseLink.name + ")</a>"
                    onLinkActivated: Qt.openUrlExternally(releaseLink.url);

                    MouseArea {
                        acceptedButtons: Qt.NoButton; cursorShape: Qt.PointingHandCursor; anchors.fill: parent
                    }
                }
            }

            Text {
                text: qsTr("Packages") + ":"
                font.bold: true
                font.pixelSize: 16
                visible: packages_repeater.count > 0
            }

            Repeater {
                id: packages_repeater
                model: release.packages

                delegate: Text {
                    property ReleaseLink releaseLink: packages_repeater.model[index]
                    x: 10
                    text: "* <a href=\"1.1\">" + releaseLink.name + "</a>"
                    onLinkActivated: Qt.openUrlExternally(releaseLink.url)

                    MouseArea {
                        acceptedButtons: Qt.NoButton; cursorShape: Qt.PointingHandCursor; anchors.fill: parent
                    }
                }
            }

            Text {
                text: qsTr("Images") + ":"
                font.bold: true
                font.pixelSize: 16
                visible: images_repeater.count > 0
            }

            Repeater {
                id: images_repeater
                model: release.images

                delegate: Text {
                    property ReleaseLink releaseLink: images_repeater.model[index]
                    x: 10
                    text: "* <a href=\"1.1\">" + releaseLink.name + "</a>"
                    onLinkActivated: Qt.openUrlExternally(releaseLink.url)

                    MouseArea {
                        acceptedButtons: Qt.NoButton; cursorShape: Qt.PointingHandCursor; anchors.fill: parent
                    }
                }
            }

            Text {
                text: qsTr("Runbooks") + ":"
                font.bold: true
                font.pixelSize: 16
                visible: runbooks_repeater.count > 0
            }

            Repeater {
                id: runbooks_repeater
                model: release.runbooks

                delegate: Text {
                    property ReleaseLink releaseLink: runbooks_repeater.model[index]
                    x: 10
                    text: "* <a href=\"1.1\">" + releaseLink.name + "</a>"
                    onLinkActivated: Qt.openUrlExternally(releaseLink.url)

                    MouseArea {
                        acceptedButtons: Qt.NoButton; cursorShape: Qt.PointingHandCursor; anchors.fill: parent
                    }
                }
            }

            Text {
                text: qsTr("Other") + ":"
                font.bold: true
                font.pixelSize: 16
                visible: other_repeater.count > 0
            }

            Repeater {
                id: other_repeater
                model: release.otherLinks

                delegate: Text {
                    property ReleaseLink releaseLink: other_repeater.model[index]
                    x: 10
                    text: "* <a href=\"1.1\">" + releaseLink.name + "</a>"
                    onLinkActivated: Qt.openUrlExternally(releaseLink.url)

                    MouseArea {
                        acceptedButtons: Qt.NoButton; cursorShape: Qt.PointingHandCursor; anchors.fill: parent
                    }
                }
            }
        }

        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                leftMargin: 10
                rightMargin: 10
                bottomMargin: 1
            }
            height: 10
        }
    }
}
