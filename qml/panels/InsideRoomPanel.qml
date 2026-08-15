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
    // Mirrors ApplicationWindow's own wideLayout formula (main.qml) -
    // computed locally since this panel is always given the full
    // available area by main.qml, so its own aspect ratio is a reliable
    // proxy for the window's. Side-by-side squeezes both halves into
    // unusable slivers on narrow windows; below this ratio, show exactly
    // one half at a time via the tab bar instead.
    readonly property bool wideLayout: width >= height * 1.15
    property int tab: 0 // 0 = setup, 1 = solution - only used when narrow
    readonly property var playerLabels: [qsTr("LEFT"), qsTr("MID"), qsTr("RIGHT")]
    readonly property var symbols2d: [Symbols.Dreieck, Symbols.Viereck, Symbols.Kreis]
    // Row 2 (Pyramide/Wuerfel/Kugel) are the 3 "pure double" shapes,
    // ordered to match symbols2d's Dreieck/Viereck/Kreis columns above -
    // row 1 holds the remaining 3 mixed-pair shapes.
    readonly property var symbols3d: [Symbols.Kegel, Symbols.Zylinder, Symbols.Prisma,
        Symbols.Pyramide, Symbols.Wuerfel, Symbols.Kugel]

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

    // Detailed mode: enter what's actually on each player's wall right now
    // (2 symbols each) and get a real LFG or Fast cleanse/distribute
    // sequence instead of assuming an already-cleansed start. UNVERIFIED
    // AGAINST REAL GAMEPLAY - see calculateinsidesteps.h and
    // fastcleanseresolver.h.
    property bool detailedMode: false
    property string cleanseMethod: "fast" // "fast" | "lfg"
    property int wall1a: 0
    property int wall1b: 0
    property int wall2a: 0
    property int wall2b: 0
    property int wall3a: 0
    property int wall3b: 0

    readonly property int stepCount: insideCalculator ? insideCalculator.numberOfSteps : 0
    // numberOfSteps only reflects the default/challenge/LFG-distribute
    // engine - it doesn't change on every calculateStepsFast() call (Fast
    // never touches that engine), so it can't reliably force a re-eval of
    // Fast-mode-only bindings below (QML only re-fires dependents when a
    // watched property's VALUE actually changes). calculationVersion is a
    // plain counter bumped on every single calculate*/reset() call, so its
    // value always differs from before - safe to depend on for this.
    readonly property int calculationVersion: insideCalculator ? insideCalculator.calculationVersion : 0
    readonly property var finalShapes: (insideCalculator && root.calculationVersion >= 0)
        ? [insideCalculator.finalShapeForPlayer(0), insideCalculator.finalShapeForPlayer(1),
           insideCalculator.finalShapeForPlayer(2)]
        : [0, 0, 0]
    readonly property int cleanseStepCount: (insideCalculator && root.calculationVersion >= 0)
        ? insideCalculator.numberOfCleanseSteps() : 0
    readonly property int fastRoundCount: (insideCalculator && root.calculationVersion >= 0)
        ? insideCalculator.numberOfFastRounds() : 0
    readonly property int fastTransferCount: (insideCalculator && root.calculationVersion >= 0)
        ? insideCalculator.numberOfFastTransfers() : 0

    function fastTransfersForRound(round) {
        var result = []
        if (!insideCalculator)
            return result
        // Read calculationVersion directly (not just fastTransferCount,
        // an intermediate derived value that can legitimately repeat
        // across two different calculations with the same transfer
        // count) so this binding always re-evaluates on recalculation.
        if (root.calculationVersion < 0)
            return result
        var total = root.fastTransferCount
        for (var i = 0; i < total; ++i) {
            if (insideCalculator.fastTransferRound(i) === round) {
                result.push({
                    from: insideCalculator.fastTransferFrom(i),
                    to: insideCalculator.fastTransferTo(i),
                    symbol: insideCalculator.fastTransferSymbol(i)
                })
            }
        }
        return result
    }

    function wallValue(idx, slot) {
        if (idx === 0) return slot === 0 ? wall1a : wall1b
        if (idx === 1) return slot === 0 ? wall2a : wall2b
        return slot === 0 ? wall3a : wall3b
    }

    function setWallValue(idx, slot, value) {
        if (idx === 0) { if (slot === 0) wall1a = value; else wall1b = value }
        else if (idx === 1) { if (slot === 0) wall2a = value; else wall2b = value }
        else { if (slot === 0) wall3a = value; else wall3b = value }
    }

    function reset() {
        player1 = 0; player2 = 0; player3 = 0
        target1 = 0; target2 = 0; target3 = 0
        wall1a = 0; wall1b = 0; wall2a = 0; wall2b = 0; wall3a = 0; wall3b = 0
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
        if (root.detailedMode) {
            var wallsSet = wall1a > 0 && wall1b > 0 && wall2a > 0 && wall2b > 0
                && wall3a > 0 && wall3b > 0
            if (!wallsSet) {
                hasNoSolution = false
                insideCalculator.reset()
                return
            }

            if (root.challengeMode) {
                // Fast's decision table is only proven for the default
                // target formula (see calculateinsidesteps.h) - Challenge
                // mode always distributes via LFG instead, whose generic
                // engine already handles arbitrary balanced targets. The
                // Fast button is disabled below while Challenge Mode is on.
                if (target1 <= 0 || target2 <= 0 || target3 <= 0) {
                    hasNoSolution = false
                    insideCalculator.reset()
                    return
                }
                if (!insideCalculator.checkIsValidWallChallenge(player1, player2, player3,
                                                                  wall1a, wall1b, wall2a, wall2b,
                                                                  wall3a, wall3b,
                                                                  target1, target2, target3)) {
                    hasNoSolution = true
                    insideCalculator.reset()
                    return
                }
                insideCalculator.calculateStepsLFGChallenge(player1, player2, player3,
                                                              wall1a, wall1b, wall2a, wall2b,
                                                              wall3a, wall3b,
                                                              target1, target2, target3)
                hasNoSolution = !(insideCalculator.isCleanseSolved() && insideCalculator.isSolved())
                return
            }

            if (!insideCalculator.checkIsValidWall(player1, player2, player3,
                                                     wall1a, wall1b, wall2a, wall2b,
                                                     wall3a, wall3b)) {
                hasNoSolution = true
                insideCalculator.reset()
                return
            }
            if (root.cleanseMethod === "lfg") {
                insideCalculator.calculateStepsLFG(player1, player2, player3,
                                                     wall1a, wall1b, wall2a, wall2b,
                                                     wall3a, wall3b)
                hasNoSolution = !(insideCalculator.isCleanseSolved() && insideCalculator.isSolved())
            } else {
                insideCalculator.calculateStepsFast(player1, player2, player3,
                                                      wall1a, wall1b, wall2a, wall2b,
                                                      wall3a, wall3b)
                hasNoSolution = !insideCalculator.isFastSolved()
            }
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
    onChallengeModeChanged: {
        // Fast isn't offered under Challenge Mode (see tryCalculate()) -
        // force LFG so cleanseMethod can't be left pointing at a method
        // that's about to be hidden/disabled in the UI below.
        if (root.challengeMode)
            root.cleanseMethod = "lfg"
        tryCalculate()
    }
    onDetailedModeChanged: tryCalculate()
    onCleanseMethodChanged: tryCalculate()
    onWall1aChanged: tryCalculate()
    onWall1bChanged: tryCalculate()
    onWall2aChanged: tryCalculate()
    onWall2bChanged: tryCalculate()
    onWall3aChanged: tryCalculate()
    onWall3bChanged: tryCalculate()

    color: "#0d0d0d"
    radius: 10
    border.color: "#2a2a2a"

    // Tab bar, narrow layout only - lets exactly one half be shown at a
    // time instead of squeezing both side by side or risking one half
    // getting pushed out of reach with no way to scroll back to it.
    Rectangle {
        id: insideTabBar
        x: 16
        y: 16
        width: parent.width - 32
        height: 32
        radius: 6
        color: "#161616"
        border.color: "#333333"
        visible: !root.wideLayout

        Row {
            anchors.fill: parent
            anchors.margins: 2

            Rectangle {
                width: parent.width / 2
                height: parent.height
                radius: 5
                color: root.tab === 0 ? "#3b82f6" : "transparent"
                Text {
                    anchors.centerIn: parent
                    text: qsTr("Setup")
                    color: root.tab === 0 ? "white" : "#999999"
                    font.pixelSize: 12
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: root.tab = 0
                }
            }
            Rectangle {
                width: parent.width / 2
                height: parent.height
                radius: 5
                color: root.tab === 1 ? "#3b82f6" : "transparent"
                Text {
                    anchors.centerIn: parent
                    text: qsTr("Solution")
                    color: root.tab === 1 ? "white" : "#999999"
                    font.pixelSize: 12
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: root.tab = 1
                }
            }
        }
    }

        Column {
            id: leftCol
            x: 16
            y: root.wideLayout ? 16 : insideTabBar.height + 24
            width: root.wideLayout ? (root.width - 24 - 32) / 2 : root.width - 32
            height: root.height - y - 16
            visible: root.wideLayout || root.tab === 0
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
                    text: qsTr("No solution for this combination - check your inputs")
                    color: "#ffb4b4"
                    font.pixelSize: 12
                }
            }

            Text {
                text: qsTr("Solo room symbols")
                color: "#ffffff"
                font.pixelSize: 18
                font.bold: true
            }

            Text {
                text: qsTr("Each player's own statue symbol")
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
                    text: qsTr("Challenge Mode")
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
                    text: qsTr("Outside escape shapes (called by outside team)")
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

            Row {
                spacing: 14

                Text {
                    text: qsTr("Detailed mode (wall input, LFG/Fast)")
                    color: "#cccccc"
                    font.pixelSize: 13
                    anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle {
                    width: 40
                    height: 22
                    radius: 11
                    anchors.verticalCenter: parent.verticalCenter
                    color: root.detailedMode ? "#3b82f6" : "#333333"

                    Rectangle {
                        width: 18
                        height: 18
                        radius: 9
                        color: "white"
                        y: 2
                        x: root.detailedMode ? parent.width - width - 2 : 2
                        Behavior on x { NumberAnimation { duration: 120 } }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.detailedMode = !root.detailedMode
                    }
                }
            }

            Column {
                width: parent.width
                spacing: 8
                visible: root.detailedMode

                Text {
                    text: qsTr("UNVERIFIED against real gameplay - sanity-check in a live run before trusting this")
                    color: "#e0a030"
                    font.pixelSize: 11
                    wrapMode: Text.WordWrap
                    width: parent.width
                }

                Row {
                    spacing: 10

                    Repeater {
                        model: [{key: "fast", label: qsTr("Fast")}, {key: "lfg", label: qsTr("LFG")}]

                        delegate: Rectangle {
                            id: methodButton
                            required property var modelData
                            // Fast isn't offered under Challenge Mode -
                            // its decision table is only proven for the
                            // default target formula (calculateinsidesteps.h).
                            readonly property bool disabledForChallenge:
                                modelData.key === "fast" && root.challengeMode
                            width: 64
                            height: 28
                            radius: 6
                            color: root.cleanseMethod === modelData.key ? "#3b82f6" : "#2a2a2a"
                            opacity: disabledForChallenge ? 0.4 : 1
                            border.color: "#444444"

                            Text {
                                anchors.centerIn: parent
                                text: methodButton.modelData.label
                                color: "#ffffff"
                                font.pixelSize: 12
                            }

                            MouseArea {
                                anchors.fill: parent
                                enabled: !methodButton.disabledForChallenge
                                onClicked: root.cleanseMethod = methodButton.modelData.key
                            }
                        }
                    }
                }

                Text {
                    visible: root.challengeMode
                    text: qsTr("Fast isn't available in Challenge Mode - using LFG")
                    color: "#777777"
                    font.pixelSize: 10
                }

                Text {
                    text: qsTr("Your wall right now (2 symbols per player)")
                    color: "#999999"
                    font.pixelSize: 11
                }

                Row {
                    id: wallRow
                    width: parent.width
                    spacing: 12

                    Repeater {
                        model: 3

                        delegate: Column {
                            id: wallCol
                            required property int index
                            width: (wallRow.width - wallRow.spacing * 2) / 3
                            spacing: 4

                            Text {
                                text: root.playerLabels[wallCol.index]
                                color: "#999999"
                                font.pixelSize: 11
                                anchors.horizontalCenter: parent.horizontalCenter
                            }

                            ShapeGridSelector {
                                columns: 3
                                totalWidth: wallCol.width
                                cellSpacing: 4
                                options: root.symbols2d
                                selected: root.wallValue(wallCol.index, 0)
                                onTapped: (value) => root.setWallValue(wallCol.index, 0, value)
                            }

                            ShapeGridSelector {
                                columns: 3
                                totalWidth: wallCol.width
                                cellSpacing: 4
                                options: root.symbols2d
                                selected: root.wallValue(wallCol.index, 1)
                                onTapped: (value) => root.setWallValue(wallCol.index, 1, value)
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
                    text: qsTr("Reset")
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
            x: root.wideLayout ? leftCol.x + leftCol.width + 24 : 16
            y: root.wideLayout ? 16 : insideTabBar.height + 24
            width: root.wideLayout ? (root.width - 24 - 32) / 2 : root.width - 32
            height: root.height - y - 16
            visible: root.wideLayout || root.tab === 1
            spacing: 14

            Text {
                id: escapeTitle
                text: qsTr("Escape shapes")
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

                    // Default / Challenge mode.
                    Repeater {
                        model: root.detailedMode ? 0 : root.stepCount

                        delegate: StepCard {
                            id: stepCard
                            required property int index
                            width: stepsColumn.width
                            stepNumber: stepCard.index + 1
                            nodeLabels: root.playerLabels
                            // Guarded on calculationVersion (not just the
                            // invokable calls below) so this binding
                            // actually re-evaluates on recalculation - a
                            // plain invokable call with no property read
                            // never re-fires on its own in QML.
                            instructions: root.calculationVersion >= 0 ? [
                                root.insideCalculator.getInstructionForStep(stepCard.index, 0),
                                root.insideCalculator.getInstructionForStep(stepCard.index, 1),
                                root.insideCalculator.getInstructionForStep(stepCard.index, 2)
                            ] : [0, 0, 0]
                            expectedState: root.finalShapes
                        }
                    }

                    // LFG: cleanse phase, sync callout, then distribute phase.
                    Column {
                        width: stepsColumn.width
                        spacing: 10
                        visible: root.detailedMode && root.cleanseMethod === "lfg"

                        Text {
                            visible: root.cleanseStepCount > 0
                            text: qsTr("CLEANSE PHASE")
                            color: "#888888"
                            font.pixelSize: 10
                            font.letterSpacing: 1
                        }

                        Repeater {
                            model: (root.detailedMode && root.cleanseMethod === "lfg") ? root.cleanseStepCount : 0

                            delegate: StepCard {
                                id: cleanseCard
                                required property int index
                                width: stepsColumn.width
                                stepNumber: cleanseCard.index + 1
                                nodeLabels: root.playerLabels
                                instructions: root.calculationVersion >= 0 ? [
                                    root.insideCalculator.getCleanseInstructionForStep(cleanseCard.index, 0),
                                    root.insideCalculator.getCleanseInstructionForStep(cleanseCard.index, 1),
                                    root.insideCalculator.getCleanseInstructionForStep(cleanseCard.index, 2)
                                ] : [0, 0, 0]
                                expectedState: [root.player1, root.player2, root.player3]
                            }
                        }

                        Rectangle {
                            visible: root.cleanseStepCount > 0
                            width: stepsColumn.width
                            height: 32
                            radius: 6
                            color: "#1a2a3a"
                            border.color: "#3b5a7a"

                            Text {
                                anchors.centerIn: parent
                                text: qsTr("Wait until all 3 players have cleansed, then distribute")
                                color: "#9dc4e8"
                                font.pixelSize: 11
                            }
                        }

                        Text {
                            visible: root.stepCount > 0
                            text: qsTr("DISTRIBUTE PHASE")
                            color: "#888888"
                            font.pixelSize: 10
                            font.letterSpacing: 1
                        }

                        Repeater {
                            model: (root.detailedMode && root.cleanseMethod === "lfg") ? root.stepCount : 0

                            delegate: StepCard {
                                id: distributeCard
                                required property int index
                                width: stepsColumn.width
                                stepNumber: distributeCard.index + 1
                                nodeLabels: root.playerLabels
                                instructions: root.calculationVersion >= 0 ? [
                                    root.insideCalculator.getInstructionForStep(distributeCard.index, 0),
                                    root.insideCalculator.getInstructionForStep(distributeCard.index, 1),
                                    root.insideCalculator.getInstructionForStep(distributeCard.index, 2)
                                ] : [0, 0, 0]
                                expectedState: root.finalShapes
                            }
                        }
                    }

                    // Fast: local decision-table rounds, no sync callout.
                    Column {
                        width: stepsColumn.width
                        spacing: 10
                        visible: root.detailedMode && root.cleanseMethod === "fast"

                        Repeater {
                            model: (root.detailedMode && root.cleanseMethod === "fast") ? root.fastRoundCount : 0

                            delegate: Column {
                                id: fastRoundBlock
                                required property int index
                                width: stepsColumn.width
                                spacing: 6

                                Text {
                                    text: qsTr("ROUND %1").arg(fastRoundBlock.index + 1)
                                    color: "#888888"
                                    font.pixelSize: 10
                                    font.letterSpacing: 1
                                }

                                Repeater {
                                    model: root.fastTransfersForRound(fastRoundBlock.index)

                                    delegate: Rectangle {
                                        required property var modelData
                                        width: stepsColumn.width
                                        height: 34
                                        radius: 6
                                        color: "#161616"
                                        border.color: "#333333"

                                        Row {
                                            anchors.left: parent.left
                                            anchors.leftMargin: 10
                                            anchors.verticalCenter: parent.verticalCenter
                                            spacing: 8

                                            Text {
                                                text: qsTr("%1 gives").arg(root.playerLabels[modelData.from])
                                                color: "#dddddd"
                                                font.pixelSize: 12
                                            }

                                            Image {
                                                width: 20
                                                height: 20
                                                fillMode: Image.PreserveAspectFit
                                                source: ShapeIcons.iconSource(modelData.symbol)
                                            }

                                            Text {
                                                text: qsTr("to %1").arg(root.playerLabels[modelData.to])
                                                color: "#dddddd"
                                                font.pixelSize: 12
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Text {
                        visible: !root.hasNoSolution && root.stepCount === 0
                                 && root.cleanseStepCount === 0 && root.fastRoundCount === 0
                        text: qsTr("Select all 3 starting symbols to see the solution.")
                        color: "#666666"
                        font.pixelSize: 12
                    }
                }
            }
        }
}
