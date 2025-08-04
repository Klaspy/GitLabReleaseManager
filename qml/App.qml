import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

import CustomStyle

import "./modal"

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
        }
    }

    PrivateKeysListDialog {id: pKeyList}
}
