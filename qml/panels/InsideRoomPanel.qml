import QtQuick
import CalculateSteps 1.0
import SymbolEnums 1.0
import "../js/ShapeIcons.js" as ShapeIcons

// Inside/solo-room puzzle: each of the 3 teleported players picks their own
// starting 2D symbol; the panel computes the cleanse/distribute swap
// sequence and each player's final escape shape.
//
// NOTE: the underlying model (CalculateInsideSteps) is not verified against
// real gameplay yet - see calculateinsidesteps.h. Treat this panel's output
// with the same caution.
Rectangle {
    id: root

    property CalculateInsideSteps insideCalculator
    readonly property var playerLabels: ["LEFT", "MID", "RIGHT"]
    readonly property var symbols2d: [Symbols.Dreieck, Symbols.Viereck, Symbols.Kreis]

    property int player1: 0
    property int player2: 0
    property int player3: 0

    readonly property int stepCount: insideCalculator ? insideCalculator.numberOfSteps : 0
    // See SolutionPanel.qml's targetShapes for why stepCount must be read
    // here too - finalShapeForPlayer() alone wouldn't trigger a re-eval.
    readonly property var finalShapes: (insideCalculator && root.stepCount >= 0)
        ? [insideCalculator.finalShapeForPlayer(0), insideCalculator.finalShapeForPlayer(1),
           insideCalculator.finalShapeForPlayer(2)]
        : [0, 0, 0]

    function reset() {
        player1 = 0; player2 = 0; player3 = 0
        insideCalculator.reset()
    }

    function tryCalculate() {
        if (player1 <= 0 || player2 <= 0 || player3 <= 0) {
            insideCalculator.reset()
            return
        }
        if (!insideCalculator.checkIsValid(player1, player2, player3)) {
            insideCalculator.reset()
            return
        }
        insideCalculator.calculateSteps(player1, player2, player3)
    }

    onPlayer1Changed: tryCalculate()
    onPlayer2Changed: tryCalculate()
    onPlayer3Changed: tryCalculate()

    color: "#0d0d0d"
    radius: 10
    border.color: "#2a2a2a"

    Row {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 24

        Column {
            width: (parent.width - 24) / 2
            spacing: 18

            Text {
                text: "Solo room symbols"
                color: "#ffffff"
                font.pixelSize: 18
                font.bold: true
            }

            Text {
                text: "Each player's own statue symbol"
                color: "#999999"
                font.pixelSize: 11
            }

            Row {
                id: playerRow
                width: parent.width
                spacing: 12

                Repeater {
                    model: 3

                    delegate: Column {
                        id: playerCol
                        required property int index
                        width: (playerRow.width - playerRow.spacing * 2) / 3
                        spacing: 6

                        Text {
                            text: root.playerLabels[playerCol.index]
                            color: "#999999"
                            font.pixelSize: 11
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        ShapeGridSelector {
                            columns: 3
                            totalWidth: playerCol.width
                            cellSpacing: 6
                            options: root.symbols2d
                            selected: playerCol.index === 0 ? root.player1
                                                             : (playerCol.index === 1 ? root.player2 : root.player3)
                            onTapped: (value) => {
                                if (playerCol.index === 0) root.player1 = value
                                else if (playerCol.index === 1) root.player2 = value
                                else root.player3 = value
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: 90
                height: 34
                radius: 6
                color: "#2a2a2a"
                border.color: "#444444"

                Text {
                    anchors.centerIn: parent
                    text: "Reset"
                    color: "#dddddd"
                    font.pixelSize: 13
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: root.reset()
                }
            }
        }

        Column {
            width: (parent.width - 24) / 2
            spacing: 14

            Text {
                text: "Escape shapes"
                color: "#ffffff"
                font.pixelSize: 18
                font.bold: true
            }

            Row {
                spacing: 14

                Repeater {
                    model: 3

                    delegate: Column {
                        id: shapeEntry
                        required property int index
                        spacing: 4

                        Text {
                            text: root.playerLabels[shapeEntry.index]
                            color: "#999999"
                            font.pixelSize: 10
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Rectangle {
                            width: 60
                            height: 60
                            radius: 6
                            color: "#161616"
                            border.color: "#333333"

                            Image {
                                anchors.centerIn: parent
                                width: 40
                                height: 40
                                fillMode: Image.PreserveAspectFit
                                source: ShapeIcons.iconSource(root.finalShapes[shapeEntry.index])
                            }
                        }
                    }
                }
            }

            Flickable {
                width: parent.width
                height: 260
                contentWidth: width
                contentHeight: stepsColumn.implicitHeight
                clip: true

                Column {
                    id: stepsColumn
                    width: parent.width
                    spacing: 10

                    Repeater {
                        model: root.stepCount

                        delegate: StepCard {
                            id: stepCard
                            required property int index
                            width: stepsColumn.width
                            stepNumber: stepCard.index + 1
                            nodeLabels: root.playerLabels
                            instructions: [
                                root.insideCalculator.getInstructionForStep(stepCard.index, 0),
                                root.insideCalculator.getInstructionForStep(stepCard.index, 1),
                                root.insideCalculator.getInstructionForStep(stepCard.index, 2)
                            ]
                            expectedState: root.finalShapes
                        }
                    }

                    Text {
                        visible: root.stepCount === 0
                        text: "Select all 3 starting symbols to see the solution."
                        color: "#666666"
                        font.pixelSize: 12
                    }
                }
            }
        }
    }
}
