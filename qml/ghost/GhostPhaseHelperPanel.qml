import QtQuick
import CalculateSteps 1.0

// Verity Ghost Phase Helper: 6-player roster for the resurrection/ghost
// phase, purely manual entry (no Bungie API - out of scope).
Rectangle {
    id: root

    property GhostPhaseHelper ghostHelper
    readonly property var positionLabels: ["LEFT", "MID", "RIGHT"]
    // Reflow to 1 column on phone-width screens so cards stay usable
    // instead of being squeezed unreadably narrow.
    readonly property int gridColumns: width < 480 ? 1 : 2

    color: "#0d0d0d"
    radius: 10
    border.color: "#2a2a2a"

    function positionLabelForIndex(index) {
        return root.positionLabels[Math.floor(index / 2)]
    }

    function copyForChat() {
        var lines = ["Verity Ghost Phase:"]
        var players = root.ghostHelper ? root.ghostHelper.players : []
        for (var i = 0; i < players.length; ++i) {
            var slot = players[i]
            var name = slot.playerName.length > 0 ? slot.playerName : "Player " + (i + 1)
            lines.push(root.positionLabelForIndex(i) + " - " + name + ": " + slot.manualNote)
        }
        chatText.text = lines.join("\n")
        chatText.selectAll()
        chatText.copy()
    }

    TextEdit {
        id: chatText
        visible: false
        text: ""
    }

    Flickable {
        anchors.fill: parent
        anchors.margins: 16
        contentWidth: width
        contentHeight: mainColumn.implicitHeight
        clip: true

        Column {
            id: mainColumn
            width: parent.width
            spacing: 14

            Row {
                width: parent.width
                height: 44
                spacing: 12

                Text {
                    text: "Verity Ghost Phase Helper"
                    color: "#ffffff"
                    font.pixelSize: 18
                    font.bold: true
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width - copyButton.width - resetButton.width - 24
                    elide: Text.ElideRight
                }

                Rectangle {
                    id: copyButton
                    width: 170
                    height: 44
                    radius: 6
                    color: "#3b82f6"
                    anchors.verticalCenter: parent.verticalCenter

                    Text {
                        anchors.centerIn: parent
                        text: "Copy ghost callouts"
                        color: "white"
                        font.pixelSize: 12
                        font.bold: true
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.copyForChat()
                    }
                }

                Rectangle {
                    id: resetButton
                    width: 90
                    height: 44
                    radius: 6
                    color: "#2a2a2a"
                    border.color: "#444444"
                    anchors.verticalCenter: parent.verticalCenter

                    Text {
                        anchors.centerIn: parent
                        text: "Reset"
                        color: "#dddddd"
                        font.pixelSize: 13
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: if (root.ghostHelper) root.ghostHelper.reset()
                    }
                }
            }

            Flow {
                id: cardFlow
                width: parent.width
                spacing: 12

                Repeater {
                    model: root.ghostHelper ? root.ghostHelper.players : []

                    delegate: GhostPlayerSlotCard {
                        id: card
                        required property var modelData
                        required property int index

                        width: root.gridColumns === 1 ? cardFlow.width
                                                       : (cardFlow.width - cardFlow.spacing) / 2
                        slot: card.modelData
                        positionLabel: root.positionLabelForIndex(card.index)
                    }
                }
            }
        }
    }
}
