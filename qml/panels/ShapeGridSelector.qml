import QtQuick
import "../js/ShapeIcons.js" as ShapeIcons

// Grid of tappable shape icons; exactly one can be selected at a time.
// Replaces the old popup-until-chosen Selection2d/Selection3dShape pattern.
Item {
    id: root

    property var options: []
    property int columns: 3
    property int selected: 0
    property real cellSpacing: 8
    // If set (>=0), cellSize is derived from this instead of the explicit
    // cellSize below - avoids fixed-pixel cells overflowing a narrower
    // parent (which would silently misalign click hit-testing).
    property real totalWidth: -1
    property real cellSize: totalWidth >= 0 ? (totalWidth - (columns - 1) * cellSpacing) / columns : 56

    // Emitted on tap; `selected` itself is meant to stay a pure one-way
    // binding from the parent (e.g. `selected: someModel.value`) - the
    // parent should update its own source-of-truth from this signal rather
    // than have this component self-assign `selected`, which would
    // permanently sever that binding (QML: an imperative write to a bound
    // property replaces the binding) and break external resets.
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

                width: root.cellSize
                height: root.cellSize
                radius: 6
                color: "#1a1a1a"
                border.color: modelData === root.selected ? "#3b82f6" : "#3a3a3a"
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
                    anchors.fill: parent
                    onClicked: root.tapped(cell.modelData)
                }
            }
        }
    }
}
