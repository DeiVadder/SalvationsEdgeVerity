import QtQuick
import "../js/ShapeIcons.js" as ShapeIcons

// Picks the 2 symbols currently on a player's wall as a single row of the
// 3 base 2D symbols - tap once to add a symbol, tap a second (different)
// symbol to complete the pair, or double-tap one symbol to mark "2 of
// this" (shown with a small badge) instead of picking 2 separate icons.
// Replaces showing the same 3-symbol grid twice (once per wall slot),
// which made an already-symmetric pair look like 2 unrelated choices.
Item {
    id: root

    property var options: []
    property int slotA: 0
    property int slotB: 0
    property real cellSpacing: 8
    property real totalWidth: -1
    readonly property int columns: options.length
    property real cellSize: totalWidth >= 0 ? (totalWidth - (columns - 1) * cellSpacing) / columns : 56

    // Emitted with the new (a, b) pair - parent owns the actual wall
    // properties, same one-way-binding convention as ShapeGridSelector.
    signal pairChanged(var a, var b)

    function countOf(symbol) {
        var count = 0
        if (root.slotA === symbol) count++
        if (root.slotB === symbol) count++
        return count
    }

    function tap(symbol) {
        var aIsSymbol = root.slotA === symbol
        var bIsSymbol = root.slotB === symbol
        if (aIsSymbol && bIsSymbol) {
            // Already doubled - a plain tap backs off to a single.
            root.pairChanged(symbol, 0)
        } else if (aIsSymbol || bIsSymbol) {
            // Already the lone pick - tap again to clear it.
            root.pairChanged(0, 0)
        } else if (root.slotA === 0 && root.slotB === 0) {
            root.pairChanged(symbol, 0)
        } else if (root.slotA !== 0 && root.slotB === 0 && root.slotA !== symbol) {
            // One different symbol already picked - this completes the pair.
            root.pairChanged(root.slotA, symbol)
        } else {
            // Pair already complete with 2 different symbols - restart.
            root.pairChanged(symbol, 0)
        }
    }

    function doubleTap(symbol) {
        root.pairChanged(symbol, symbol)
    }

    implicitWidth: grid.implicitWidth
    implicitHeight: grid.implicitHeight

    Row {
        id: grid
        spacing: root.cellSpacing

        Repeater {
            model: root.options

            delegate: Rectangle {
                id: cell
                required property var modelData
                readonly property int count: root.countOf(cell.modelData)

                width: root.cellSize
                height: root.cellSize
                radius: 6
                color: "#1a1a1a"
                border.color: cell.count > 0 ? "#3b82f6" : "#3a3a3a"
                border.width: cell.count > 0 ? 3 : 1

                Image {
                    anchors.centerIn: parent
                    width: parent.width - 16
                    height: parent.height - 16
                    source: ShapeIcons.iconSource(cell.modelData)
                    fillMode: Image.PreserveAspectFit
                    sourceSize.width: width
                    sourceSize.height: height
                }

                Rectangle {
                    width: 18
                    height: 18
                    radius: 9
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.margins: 3
                    color: "#eab308"
                    visible: cell.count === 2

                    Text {
                        anchors.centerIn: parent
                        text: "2"
                        color: "#161616"
                        font.bold: true
                        font.pixelSize: 11
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: root.tap(cell.modelData)
                    onDoubleClicked: root.doubleTap(cell.modelData)
                }
            }
        }
    }
}
