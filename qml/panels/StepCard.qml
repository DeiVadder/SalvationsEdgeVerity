import QtQuick
import "../js/ShapeIcons.js" as ShapeIcons

// One computed step: which two nodes swap what, plus the resulting state.
Rectangle {
    id: root

    property int stepNumber: 1
    property var nodeLabels: [qsTr("LEFT"), qsTr("MID"), qsTr("RIGHT")]
    // instructions[i] = symbol handed to node i this step, or 0 if untouched
    property var instructions: [0, 0, 0]
    // targetState[i] = the shape/symbol node i needs to end up as overall
    // (not a per-step intermediate state - showing the true intermediate
    // state after each individual step would need replaying the swap
    // sequence, deferred as a follow-up)
    property var expectedState: [0, 0, 0]
    // Node index to call out as "you" (e.g. the Inside panel's own solo
    // statue) - tints that column in both rows below. -1 = no highlight,
    // used by the Outside panel where every node is equally "the team".
    property int highlightIndex: -1
    // Tap-to-check-off state, purely local UI memory (not persisted, not
    // read by any calculation) - lets a player tick off steps as they
    // physically complete them without losing their place mid-encounter.
    property bool done: false

    readonly property var activeIndices: {
        var idx = []
        for (var i = 0; i < instructions.length; i++)
            if (instructions[i] !== 0)
                idx.push(i)
        return idx
    }

    color: "#161616"
    radius: 8
    border.color: root.done ? "#2e7d46" : "#333333"
    border.width: root.done ? 2 : 1
    opacity: root.done ? 0.55 : 1
    implicitWidth: 320
    implicitHeight: content.implicitHeight + 24

    Behavior on opacity { NumberAnimation { duration: 120 } }
    Behavior on border.color { ColorAnimation { duration: 120 } }

    MouseArea {
        id: doneArea
        anchors.fill: parent
        onClicked: root.done = !root.done
    }

    // Checkmark badge, top-right corner - only real feedback needed since
    // border + dimming already read as "done" but could be missed at a
    // glance while scanning down a list of cards.
    Rectangle {
        width: 22
        height: 22
        radius: 11
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 6
        color: "#2e7d46"
        visible: root.done

        Text {
            anchors.centerIn: parent
            text: "✓"
            color: "#ffffff"
            font.bold: true
            font.pixelSize: 13
        }
    }

    // Step number stretched across the whole card height, with a clear
    // vertical divider to its right - reads as a single anchor point for
    // the step instead of competing with the swap/target rows for space.
    Text {
        id: numberText
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.leftMargin: 14
        width: 26
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        text: root.stepNumber
        color: "#3b82f6"
        font.bold: true
        font.pixelSize: 20
    }

    Rectangle {
        id: numberDivider
        anchors.left: numberText.right
        anchors.leftMargin: 12
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 1
        color: "#333333"
    }

    Column {
        id: content
        anchors.left: numberDivider.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 12
        anchors.leftMargin: 14
        spacing: 10

        // Both this row and the TARGET SHAPE row below split the same
        // width into the same 3 node-indexed columns, so a carry here
        // lines up directly above/below that node's eventual target -
        // makes the connection visible instead of needing to match up
        // labels by reading.
        Row {
            id: swapRow
            width: parent.width

            Repeater {
                model: root.instructions.length

                delegate: Item {
                    id: swapEntry
                    required property int index
                    width: swapRow.width / root.instructions.length
                    height: swapContent.implicitHeight

                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: -4
                        radius: 6
                        color: "#1e3a5f"
                        visible: swapEntry.index === root.highlightIndex
                    }

                    Row {
                        id: swapContent
                        visible: root.instructions[swapEntry.index] !== 0
                        spacing: 6

                        Image {
                            width: 20
                            height: 20
                            source: ShapeIcons.iconSource(root.instructions[swapEntry.index])
                            fillMode: Image.PreserveAspectFit
                        }
                        Text {
                            text: qsTr("%1 → %2").arg(ShapeIcons.shapeName(root.instructions[swapEntry.index]))
                                                       .arg(root.nodeLabels[swapEntry.index])
                            color: "#dddddd"
                            font.pixelSize: 13
                        }
                    }
                }
            }
        }

        Rectangle {
            width: parent.width
            height: 1
            color: "#2a2a2a"
        }

        Text {
            text: qsTr("TARGET SHAPE")
            color: "#888888"
            font.pixelSize: 10
            font.letterSpacing: 1
        }

        Row {
            id: targetRow
            width: parent.width

            Repeater {
                model: root.expectedState.length

                delegate: Item {
                    id: stateEntry
                    required property int index
                    width: targetRow.width / root.expectedState.length
                    height: stateContent.implicitHeight

                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: -4
                        radius: 6
                        color: "#1e3a5f"
                        visible: stateEntry.index === root.highlightIndex
                    }

                    Row {
                        id: stateContent
                        spacing: 4

                        Image {
                            width: 18
                            height: 18
                            source: ShapeIcons.iconSource(root.expectedState[stateEntry.index])
                            fillMode: Image.PreserveAspectFit
                        }
                        Text {
                            text: ShapeIcons.shapeName(root.expectedState[stateEntry.index])
                            color: "#cccccc"
                            font.pixelSize: 12
                        }
                    }
                }
            }
        }
    }
}
