import QtQuick
import CalculateSteps 1.0
import "../js/ShapeIcons.js" as ShapeIcons

// Outside-puzzle output: target statue shapes + the computed swap sequence.
Rectangle {
    id: root

    property CalculateSteps stepCalculator
    readonly property var statueLabels: ["LEFT", "MID", "RIGHT"]

    readonly property int stepCount: stepCalculator ? stepCalculator.numberOfSteps : 0
    // targetShapeForStatue()/getInstructionForStep() are plain method
    // calls, not bindable properties - QML can't tell a binding needs to
    // re-run when the underlying data changes unless the expression also
    // reads an actual NOTIFY-equipped property. stepCount alone isn't
    // reliable for that: two different valid input combinations can
    // produce the same step count with different actual instructions, and
    // QML only re-fires a dependent binding when a watched property's
    // VALUE changes, not merely when its NOTIFY signal fires unchanged.
    // calculationVersion is a plain counter bumped on every single
    // calculateSteps()/reset() call, so its value always differs from
    // before - safe to depend on for this trick.
    readonly property int calculationVersion: stepCalculator ? stepCalculator.calculationVersion : 0
    readonly property var targetShapes: (stepCalculator && root.calculationVersion >= 0)
        ? [stepCalculator.targetShapeForStatue(0), stepCalculator.targetShapeForStatue(1),
           stepCalculator.targetShapeForStatue(2)]
        : [0, 0, 0]

    function copyForChat() {
        var lines = ["Verity dissection:"]
        for (var s = 0; s < stepCount; s++) {
            var parts = []
            for (var i = 0; i < 3; i++) {
                var sym = stepCalculator.getInstructionForStep(s, i)
                if (sym !== 0)
                    parts.push(root.statueLabels[i] + " <- " + ShapeIcons.shapeName(sym))
            }
            lines.push("Step " + (s + 1) + ": " + parts.join(", "))
        }
        chatText.text = lines.join("\n")
        chatText.selectAll()
        chatText.copy()
    }

    color: "#0d0d0d"
    radius: 10
    border.color: "#2a2a2a"

    TextEdit {
        id: chatText
        visible: false
        text: ""
    }

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 14

        Text {
            text: "Solution"
            color: "#ffffff"
            font.pixelSize: 18
            font.bold: true
        }

        Text {
            text: "Statue positions"
            color: "#999999"
            font.pixelSize: 11
        }

        Row {
            spacing: 14

            Repeater {
                model: 3

                delegate: Column {
                    id: posEntry
                    required property int index
                    spacing: 4

                    Text {
                        text: root.statueLabels[posEntry.index]
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
                            source: ShapeIcons.iconSource(root.targetShapes[posEntry.index])
                        }
                    }
                }
            }
        }

        Flickable {
            width: parent.width
            height: 320
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
                        nodeLabels: root.statueLabels
                        instructions: root.calculationVersion >= 0 ? [
                            root.stepCalculator.getInstructionForStep(stepCard.index, 0),
                            root.stepCalculator.getInstructionForStep(stepCard.index, 1),
                            root.stepCalculator.getInstructionForStep(stepCard.index, 2)
                        ] : [0, 0, 0]
                        expectedState: root.targetShapes
                    }
                }

                Text {
                    visible: root.stepCount === 0
                    text: "Select all shapes to see the solution."
                    color: "#666666"
                    font.pixelSize: 12
                }
            }
        }

        Rectangle {
            width: 220
            height: 38
            radius: 6
            color: root.stepCount > 0 ? "#3b82f6" : "#2a2a2a"

            Text {
                anchors.centerIn: parent
                text: "Copy for in-game chat"
                color: root.stepCount > 0 ? "white" : "#777777"
                font.pixelSize: 13
                font.bold: true
            }

            MouseArea {
                anchors.fill: parent
                enabled: root.stepCount > 0
                onClicked: root.copyForChat()
            }
        }
    }
}
