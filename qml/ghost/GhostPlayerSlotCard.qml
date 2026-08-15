import QtQuick
import QtQuick.Controls

// One player box: name, class, and a free-text ghost/fashion note.
Rectangle {
    id: root

    property var slot
    property string positionLabel: ""
    // Named slotIndex (not index) deliberately - the delegate that
    // instantiates this also needs its own `required property int index`
    // for Repeater's injection (see GhostPhaseHelperPanel.qml), and two
    // same-named "index" properties (one inherited, one redeclared at the
    // instantiation site) don't unify - the inner one silently stays at
    // its default forever, which is why every card showed "PLAYER 1"
    // regardless of position until this was split into two named props.
    property int slotIndex: 0

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
            text: "PLAYER " + (root.slotIndex + 1) + " / " + root.positionLabel
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
