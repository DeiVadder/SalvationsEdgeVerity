import QtQuick
import QtQuick.Controls
import "../js/ShapeIcons.js" as ShapeIcons
import "../js/ShapeMath.js" as ShapeMath

// Grid of tappable shape icons; exactly one can be selected at a time.
// Replaces the old popup-until-chosen Selection2d/Selection3dShape pattern.
Item {
    id: root

    property var options: []
    property int columns: 3
    property int selected: 0
    // True when `selected` was auto-filled by the parent (inferred, not
    // tapped) - rendered amber instead of blue so it reads as a guess.
    property bool selectionIsInferred: false
    property real cellSpacing: 8
    // If set (>=0), cellSize derives from this instead - avoids fixed-pixel
    // cells overflowing a narrower parent.
    property real totalWidth: -1
    property real cellSize: totalWidth >= 0 ? (totalWidth - (columns - 1) * cellSpacing) / columns : 56

    // `selected` stays a pure one-way binding from the parent - this signal
    // lets the parent update its own source of truth instead of us
    // self-assigning `selected`, which would sever that binding.
    signal tapped(var value)

    implicitWidth: grid.implicitWidth
    implicitHeight: grid.implicitHeight

    Grid {
        id: grid
        columns: root.columns
        spacing: root.cellSpacing

        Repeater {
            model: root.options

            delegate: Rectangle {
                id: cell
                required property var modelData

                // 3D shapes (4-9) combine 2 of the 3 base 2D symbols - shown
                // on hover since it's not otherwise visible once picked.
                readonly property bool is3dShape: cell.modelData >= 4
                readonly property var baseSymbols: cell.is3dShape ? ShapeMath.baseSymbolsFor(cell.modelData) : []

                width: root.cellSize
                height: root.cellSize
                radius: 6
                color: "#1a1a1a"
                border.color: modelData === root.selected
                              ? (root.selectionIsInferred ? "#eab308" : "#3b82f6")
                              : "#3a3a3a"
                border.width: modelData === root.selected ? 3 : 1

                Image {
                    anchors.centerIn: parent
                    width: parent.width - 16
                    height: parent.height - 16
                    source: ShapeIcons.iconSource(cell.modelData)
                    fillMode: Image.PreserveAspectFit
                    sourceSize.width: width
                    sourceSize.height: height
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.tapped(cell.modelData)
                }

                ToolTip.visible: cell.is3dShape && mouseArea.containsMouse
                ToolTip.delay: 400
                ToolTip.text: cell.baseSymbols.length === 2
                    ? qsTr("%1 + %2").arg(ShapeIcons.shapeName(cell.baseSymbols[0]))
                                     .arg(ShapeIcons.shapeName(cell.baseSymbols[1]))
                    : ""
            }
        }
    }
}
