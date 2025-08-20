import QtQuick
import QtQuick.Controls
import QtQuick.Shapes
import QtQml

ComboBox {
    id: control
    property alias borderColor: bg.border.color
    property alias bgColor: bg.color
    property alias textColor: content.color

    onModelChanged: currentIndexChanged()
    onCurrentIndexChanged: displayText = textAt(currentIndex)
    onActivated: if (editable) content.item.focus = false

    height: 30
    width: 100

    background: Rectangle {
        id: bg
        radius: 4
        border.width: 1
        border.color: "blue"
    }

    indicator: Canvas {
        id: canvas
        x: control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        height: control.height * 0.4
        width: height * 2 / Math.sqrt(3)
        contextType: "2d"

        Connections {
            target: control
            function onPressedChanged() { canvas.requestPaint(); }
        }

        onPaint: {
            var context = getContext("2d")
            context.reset();
            context.moveTo(0, 0);
            context.lineTo(width, 0);
            context.lineTo(width / 2, height);
            context.closePath();
            context.fillStyle = control.textColor;
            context.fill();
        }
    }

    contentItem: Loader {
        id: content
        sourceComponent: control.editable ? textEdit_comp : text_comp
        property color color: "black"
        property string text

        Component {
            id: text_comp
            Text {
                text: control.displayText
                verticalAlignment: Text.AlignVCenter
                leftPadding: 6
                color: content.color
                font: control.font
            }
        }

        Component {
            id: textEdit_comp
            TextEdit {
                id: textEdit
                text: control.displayText
                verticalAlignment: Text.AlignVCenter
                leftPadding: 6
                color: content.color
                font: control.font

                onFocusChanged: if (focus) control.popup.open()

                Binding {target: content; property: "text"; value: text}
                Binding on text {when: !focus && !control.popup.visible; value: control.displayText}

                Connections {
                    target: control.popup

                    function onClosed() {textEdit.focus = false}
                    function onOpened() {if (control.editable) textEdit.text = ""}
                }

            }
        }
    }

    property string enabledRole: ""
    delegate: ItemDelegate {
        id: delegate
        required property var model
        required property int index
        width: control.width
        height: txt.visible ? 24 : 0
        background: Rectangle {
            color: control.highlightedIndex == index ? "#f0f0f0" : "white"
        }
        enabled: control.enabledRole == "" || model[control.enabledRole]

        states: [
            State {
                name: "highlighted"
                when: enabled && control.highlightedIndex == index
                PropertyChanges {target: delegate.background; color: "#f0f0f0"}
            },

            State {
                name: "disabled"
                when: !enabled
                PropertyChanges {target: delegate.background; color: "gray"}
            }

        ]

        Text {
            id: txt
            text: delegate.model[control.textRole]
            leftPadding: content.leftPadding
            anchors.verticalCenter: parent.verticalCenter
            color: control.textColor
            font: control.font
            visible: !control.editable || content.text === "" || text.includes(content.text)
        }
    }

    states: [
        State {
            name: "down"
            when: control.down
            PropertyChanges {target: canvas; rotation: 180}
        }
    ]
}
