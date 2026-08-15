import QtQuick
import QtQuick.Controls

// One player box: name, class, and a free-text ghost/fashion note.
Rectangle {
    id: root

    property var slot
    property string positionLabel: ""
    property int index: 0

    color: "#161616"
    radius: 8
    border.color: "#333333"
    implicitHeight: content.implicitHeight + 24

    Column {
        id: content
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 12
        spacing: 8

        Text {
            text: "PLAYER " + (root.index + 1) + " / " + root.positionLabel
            color: "#999999"
            font.pixelSize: 11
            font.bold: true
        }

        TextField {
            width: parent.width
            height: 44
            placeholderText: "Player name"
            text: root.slot ? root.slot.playerName : ""
            onTextEdited: if (root.slot) root.slot.playerName = text
        }

        ComboBox {
            width: parent.width
            height: 44
            model: ["Titan", "Hunter", "Warlock"]
            currentIndex: root.slot ? root.slot.classType : 0
            onActivated: (idx) => { if (root.slot) root.slot.classType = idx }
        }

        TextField {
            width: parent.width
            height: 44
            placeholderText: "Ghost or fashion note"
            text: root.slot ? root.slot.manualNote : ""
            onTextEdited: if (root.slot) root.slot.manualNote = text
        }
    }
}
