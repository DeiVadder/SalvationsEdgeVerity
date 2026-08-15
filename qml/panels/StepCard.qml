import QtQuick
import "../js/ShapeIcons.js" as ShapeIcons

// One computed step: which two nodes swap what, plus the resulting state.
Rectangle {
    id: root

    property int stepNumber: 1
    property var nodeLabels: ["LEFT", "MID", "RIGHT"]
    // instructions[i] = symbol handed to node i this step, or 0 if untouched
    property var instructions: [0, 0, 0]
    // targetState[i] = the shape/symbol node i needs to end up as overall
    // (not a per-step intermediate state - showing the true intermediate
    // state after each individual step would need replaying the swap
    // sequence, deferred as a follow-up)
    property var expectedState: [0, 0, 0]

    readonly property var activeIndices: {
        var idx = []
        for (var i = 0; i < instructions.length; i++)
            if (instructions[i] !== 0)
                idx.push(i)
        return idx
    }

    color: "#161616"
    radius: 8
    border.color: "#333333"
    border.width: 1
    implicitWidth: 320
    implicitHeight: content.implicitHeight + 24

    Column {
        id: content
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 12
        spacing: 10

        Row {
            spacing: 10

            Rectangle {
                width: 26
                height: 26
                radius: 13
                color: "#3b82f6"
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    anchors.centerIn: parent
                    text: root.stepNumber
                    color: "white"
                    font.bold: true
                    font.pixelSize: 13
                }
            }

            Repeater {
                model: root.activeIndices

                delegate: Row {
                    id: swapEntry
                    required property var modelData
                    required property int index
                    spacing: 6
                    anchors.verticalCenter: parent.verticalCenter

                    Image {
                        width: 20
                        height: 20
                        source: ShapeIcons.iconSource(root.instructions[swapEntry.modelData])
                        fillMode: Image.PreserveAspectFit
                    }
                    Text {
                        text: root.nodeLabels[swapEntry.modelData]
                        color: "#dddddd"
                        font.pixelSize: 13
                    }
                    Text {
                        visible: swapEntry.index < root.activeIndices.length - 1
                        text: "⇄"
                        color: "#888888"
                        font.pixelSize: 14
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
            text: "TARGET SHAPE"
            color: "#888888"
            font.pixelSize: 10
            font.letterSpacing: 1
        }

        Row {
            spacing: 14

            Repeater {
                model: root.expectedState.length

                delegate: Row {
                    id: stateEntry
                    required property int index
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
