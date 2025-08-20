import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Shapes
import QtQuick.Effects

import qmlcomponents
import CustomStyle

import "../modal"

Item {
    id: releases_root

    property Project project

    onVisibleChanged: newRelease_dialog.reject()

    NewReleaseDialog {
        id: newRelease_dialog
        project: releases_root.project
    }

    Column {
        width: releases_root.width
        spacing: 20
        visible: !newRelease_dialog.visible

        Row {
            layoutDirection: Qt.RightToLeft
            width: parent.width - 5

            Button {
                text: qsTr("New release")
                bgColor: "dodgerblue"
                textColor: "white"
                enabled: project !== null && project.accessLevel >= GitLabAccessLevels.Developer
                onClicked: newRelease_dialog.open()
            }
        }

        ScrollView {
            width: parent.width
            height: releases_root.height - y

            Column {
                width: releases_root.width
                spacing: 20

                Repeater {
                    model: project != null ? project.releases : undefined

                    delegate: ReleaseColumnElement {
                        id: release_col
                        accessLevel: project.accessLevel
                    }
                }
            }
        }
    }
}
