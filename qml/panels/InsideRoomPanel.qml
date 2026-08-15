import QtQuick
import CalculateSteps 1.0
import SymbolEnums 1.0
import "../js/ShapeIcons.js" as ShapeIcons
import "../js/ShapeMath.js" as ShapeMath

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
    property var encounterProgress
    readonly property var playerLabels: ["LEFT", "MID", "RIGHT"]
    readonly property var symbols2d: [Symbols.Dreieck, Symbols.Viereck, Symbols.Kreis]
    readonly property var symbols3d: [Symbols.Kegel, Symbols.Wuerfel, Symbols.Zylinder,
        Symbols.Pyramide, Symbols.Prisma, Symbols.Kugel]

    property int player1: 0
    property int player2: 0
    property int player3: 0

    // Same "2 distinct picks force the 3rd" inference as InputPanel's inside
    // symbols - the 3 solo statues always show pairwise-distinct symbols.
    property int inferredPlayerIndex: -1
    property bool hasNoSolution: false

    readonly property bool challengeMode: root.encounterProgress
        ? root.encounterProgress.challengeModeEnabled : false

    // The outside caller's 3 outside 3D shapes, relayed verbally to the
    // inside team - only used/shown in challenge mode.
    property int target1: 0
    property int target2: 0
    property int target3: 0
    property int inferredTargetIndex: -1

    readonly property int stepCount: insideCalculator ? insideCalculator.numberOfSteps : 0
    // See SolutionPanel.qml's targetShapes for why stepCount must be read
    // here too - finalShapeForPlayer() alone wouldn't trigger a re-eval.
    readonly property var finalShapes: (insideCalculator && root.stepCount >= 0)
        ? [insideCalculator.finalShapeForPlayer(0), insideCalculator.finalShapeForPlayer(1),
           insideCalculator.finalShapeForPlayer(2)]
        : [0, 0, 0]

    function reset() {
        player1 = 0; player2 = 0; player3 = 0
        target1 = 0; target2 = 0; target3 = 0
        inferredPlayerIndex = -1
        inferredTargetIndex = -1
        hasNoSolution = false
        insideCalculator.reset()
    }

    function targetValue(idx) {
        return idx === 0 ? target1 : (idx === 1 ? target2 : target3)
    }

    function setTargetValue(idx, value) {
        if (idx === 0) target1 = value
        else if (idx === 1) target2 = value
        else target3 = value
    }

    function setTarget(idx, value) {
        if (idx === inferredTargetIndex) {
            inferredTargetIndex = -1
        } else if (inferredTargetIndex >= 0) {
            setTargetValue(inferredTargetIndex, 0)
            inferredTargetIndex = -1
        }
        setTargetValue(idx, value)
        maybeInferMissingTarget()
        tryCalculate()
    }

    function maybeInferMissingTarget() {
        var vals = [target1, target2, target3]
        var setIdx = []
        var zeroIdx = -1
        for (var i = 0; i < 3; ++i) {
            if (vals[i] > 0)
                setIdx.push(i)
            else
                zeroIdx = i
        }
        if (setIdx.length === 2 && zeroIdx >= 0) {
            var remaining = ShapeMath.remainingBasePair(vals[setIdx[0]], vals[setIdx[1]])
            if (remaining.length === 2) {
                var inferredShape = ShapeMath.shapeForBasePair(remaining[0], remaining[1])
                if (inferredShape > 0) {
                    setTargetValue(zeroIdx, inferredShape)
                    inferredTargetIndex = zeroIdx
                }
            }
        } else if (setIdx.length < 2) {
            inferredTargetIndex = -1
        }
    }

    function playerValue(idx) {
        return idx === 0 ? player1 : (idx === 1 ? player2 : player3)
    }

    function setPlayerValue(idx, value) {
        if (idx === 0) player1 = value
        else if (idx === 1) player2 = value
        else player3 = value
    }

    function setPlayer(idx, value) {
        if (idx === inferredPlayerIndex)
            inferredPlayerIndex = -1
        if (inferredPlayerIndex >= 0 && playerValue(inferredPlayerIndex) === value) {
            setPlayerValue(inferredPlayerIndex, 0)
            inferredPlayerIndex = -1
        }
        setPlayerValue(idx, value)
        maybeInferMissingPlayer()
    }

    function maybeInferMissingPlayer() {
        var vals = [player1, player2, player3]
        var setIdx = []
        var zeroIdx = -1
        for (var i = 0; i < 3; ++i) {
            if (vals[i] > 0)
                setIdx.push(i)
            else
                zeroIdx = i
        }
        if (setIdx.length === 2 && zeroIdx >= 0 && vals[setIdx[0]] !== vals[setIdx[1]]) {
            var all = [Symbols.Dreieck, Symbols.Viereck, Symbols.Kreis]
            var remaining = all.filter(function (v) {
                return v !== vals[setIdx[0]] && v !== vals[setIdx[1]]
            })
            if (remaining.length === 1) {
                setPlayerValue(zeroIdx, remaining[0])
                inferredPlayerIndex = zeroIdx
            }
        } else if (setIdx.length < 2) {
            inferredPlayerIndex = -1
        }
    }

    function tryCalculate() {
        if (player1 <= 0 || player2 <= 0 || player3 <= 0) {
            hasNoSolution = false
            insideCalculator.reset()
            return
        }
        if (root.challengeMode) {
            if (target1 <= 0 || target2 <= 0 || target3 <= 0) {
                hasNoSolution = false
                insideCalculator.reset()
                return
            }
            if (!insideCalculator.checkIsValidChallenge(player1, player2, player3,
                                                          target1, target2, target3)) {
                hasNoSolution = true
                insideCalculator.reset()
                return
            }
            insideCalculator.calculateStepsChallenge(player1, player2, player3,
                                                       target1, target2, target3)
            hasNoSolution = !insideCalculator.isSolved()
            return
        }
        if (!insideCalculator.checkIsValid(player1, player2, player3)) {
            hasNoSolution = true
            insideCalculator.reset()
            return
        }
        insideCalculator.calculateSteps(player1, player2, player3)
        hasNoSolution = !insideCalculator.isSolved()
    }

    onPlayer1Changed: tryCalculate()
    onPlayer2Changed: tryCalculate()
    onPlayer3Changed: tryCalculate()
    onChallengeModeChanged: tryCalculate()

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

            Rectangle {
                width: parent.width
                height: 36
                radius: 6
                color: "#3a1a1a"
                border.color: "#a33"
                visible: root.hasNoSolution

                Text {
                    anchors.centerIn: parent
                    text: "No solution for this combination - check your inputs"
                    color: "#ffb4b4"
                    font.pixelSize: 12
                }
            }

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
                            selectionIsInferred: playerCol.index === root.inferredPlayerIndex
                            onTapped: (value) => root.setPlayer(playerCol.index, value)
                        }
                    }
                }
            }

            Row {
                spacing: 14
                visible: root.encounterProgress !== null && root.encounterProgress !== undefined

                Text {
                    text: "Challenge Mode"
                    color: "#cccccc"
                    font.pixelSize: 13
                    anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle {
                    width: 40
                    height: 22
                    radius: 11
                    anchors.verticalCenter: parent.verticalCenter
                    color: root.challengeMode ? "#3b82f6" : "#333333"

                    Rectangle {
                        width: 18
                        height: 18
                        radius: 9
                        color: "white"
                        y: 2
                        x: root.challengeMode ? parent.width - width - 2 : 2
                        Behavior on x { NumberAnimation { duration: 120 } }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (root.encounterProgress)
                                root.encounterProgress.challengeModeEnabled = !root.encounterProgress.challengeModeEnabled
                        }
                    }
                }
            }

            Column {
                width: parent.width
                spacing: 6
                visible: root.challengeMode

                Text {
                    text: "Outside escape shapes (called by outside team)"
                    color: "#999999"
                    font.pixelSize: 11
                    wrapMode: Text.WordWrap
                    width: parent.width
                }

                Row {
                    id: targetRow
                    width: parent.width
                    spacing: 12

                    Repeater {
                        model: 3

                        delegate: Column {
                            id: targetCol
                            required property int index
                            width: (targetRow.width - targetRow.spacing * 2) / 3
                            spacing: 6

                            Text {
                                text: root.playerLabels[targetCol.index]
                                color: "#999999"
                                font.pixelSize: 11
                                anchors.horizontalCenter: parent.horizontalCenter
                            }

                            ShapeGridSelector {
                                columns: 3
                                totalWidth: targetCol.width
                                cellSpacing: 6
                                options: root.symbols3d
                                selected: targetCol.index === 0 ? root.target1
                                                                 : (targetCol.index === 1 ? root.target2 : root.target3)
                                selectionIsInferred: targetCol.index === root.inferredTargetIndex
                                onTapped: (value) => root.setTarget(targetCol.index, value)
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
            id: rightCol
            width: (parent.width - 24) / 2
            height: parent.height
            spacing: 14

            Text {
                id: escapeTitle
                text: "Escape shapes"
                color: "#ffffff"
                font.pixelSize: 18
                font.bold: true
            }

            Row {
                id: shapesRow
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
                // Fill whatever's left in the panel instead of a fixed
                // height - a fixed box was clipping/hiding later steps
                // once there were more than ~5-6 of them.
                height: Math.max(120, rightCol.height - escapeTitle.height - shapesRow.height
                                       - rightCol.spacing * 2)
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
