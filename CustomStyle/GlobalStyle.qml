pragma Singleton
import QtQuick 2.15

QtObject {
    property color controlTextColor: "black"
    property color controlBgColor: _Cyan
    property font  controlFont: appFont

    // constnt fonts
    readonly property font appFont: Qt.font({
                                                pixelSize: 12,
                                                family: "Arial"
                                            })

    // constant colors
    readonly property color _Cyan: "#00f7ff"
}
