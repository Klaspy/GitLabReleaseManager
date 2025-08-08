import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtCore

import qmlcomponents
import CustomStyle

Dialog {
    id: dialog
    title: qsTr("Preferences")
    anchors.centerIn: parent
    width: 300
    height: 400

    Settings {
        id: settings

        location: StandardPaths.writableLocation(StandardPaths.AppLocalDataLocation) + "/settings.ini"
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

    Grid {
        anchors.fill: parent
        anchors.margins: 10
        columns: 2
        spacing: 20
        verticalItemAlignment: Grid.AlignVCenter

        Text {
            text: qsTr("GitLab url:")
        }

        TextField {
            width: parent.width - x

            onEditingFinished: {
                focus = false
                settings.setValue("gitLabUrl", text)
            }

            Component.onCompleted: text = settings.value("gitLabUrl") !== undefined ?
                                       settings.value("gitLabUrl") : ""
        }
    }
}
