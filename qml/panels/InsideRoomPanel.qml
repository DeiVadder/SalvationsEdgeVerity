import QtQuick
import CalculateSteps 1.0
import SymbolEnums 1.0
import "../js/ShapeIcons.js" as ShapeIcons
import "../js/ShapeMath.js" as ShapeMath

// Inside/solo-room puzzle: each of the 3 players picks their own starting
// 2D symbol; computes the sort/distribute sequence + final escape shape.
// UNVERIFIED against real gameplay - see calculateinsidesteps.h.
// 2-frame layout (setup/solution, 42/58 split) matching the outside
// puzzle's InputPanel/SolutionPanel.
Item {
    id: root

    property CalculateInsideSteps insideCalculator
    property var encounterProgress
    // Mirrors ApplicationWindow's wideLayout formula - side-by-side would
    // squeeze both halves into unusable slivers below this ratio.
    readonly property bool wideLayout: width >= height * 1.15
    property int tab: 0 // 0 = setup, 1 = solution - only used when narrow

    // Auto-advance to Solution once real steps exist (narrow layout only).
    // mySteps, not stepCount>0 - that only means own symbols are valid,
    // not that anything's actually ready to show yet. Bool, not the raw
    // count, so re-solving while already valid doesn't yank the tab back
    // every time - only the false->true transition does.
    readonly property bool hasAnySolution: mySteps.length > 0
    onHasAnySolutionChanged: {
        if (hasAnySolution && !wideLayout)
            tab = 1
    }
    readonly property var playerLabels: [qsTr("LEFT"), qsTr("MID"), qsTr("RIGHT")]
    readonly property var symbols2d: [Symbols.Dreieck, Symbols.Viereck, Symbols.Kreis]
    // Row 2 (Pyramide/Wuerfel/Kugel) are the 3 "pure double" shapes,
    // ordered to match symbols2d's Dreieck/Viereck/Kreis columns above -
    // row 1 holds the remaining 3 mixed-pair shapes.
    readonly property var symbols3d: [Symbols.Kegel, Symbols.Zylinder, Symbols.Prisma,
        Symbols.Pyramide, Symbols.Wuerfel, Symbols.Kugel]
    readonly property var pureSymbols3d: [Symbols.Pyramide, Symbols.Wuerfel, Symbols.Kugel]

    // Default distribute always builds a mixed shape, so Challenge only
    // ever needs to redirect toward a pure one - restrict the picker to
    // those 3. On by default; uncheck for the full 6-shape picker.
    property bool pureShapesOnly: true
    readonly property var targetShapeOptions: root.pureShapesOnly ? root.pureSymbols3d : root.symbols3d

    property int player1: 0
    property int player2: 0
    property int player3: 0

    // Which statue/room the app's user is personally standing at - purely
    // a display hint (highlights "your" column/step everywhere below), the
    // solver itself treats all 3 nodes identically. -1 = not marked yet.
    property int myPosition: -1

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

    // What's currently on the app user's own wall (2 symbols) - drives the
    // sort/distribute sequence. UNVERIFIED AGAINST REAL GAMEPLAY - see
    // calculateinsidesteps.h.
    property int wall1a: 0
    property int wall1b: 0
    property int wall2a: 0
    property int wall2b: 0
    property int wall3a: 0
    property int wall3b: 0

    readonly property int stepCount: insideCalculator ? insideCalculator.numberOfSteps : 0
    // Plain method calls (finalShapeForPlayer() etc.) aren't bindable -
    // calculationVersion is a plain counter bumped on every single
    // calculate*/reset() call, so QML always sees it change and re-runs
    // dependent bindings, unlike numberOfSteps which can legitimately
    // repeat across two different calculations.
    readonly property int calculationVersion: insideCalculator ? insideCalculator.calculationVersion : 0
    readonly property var finalShapes: (insideCalculator && root.calculationVersion >= 0)
        ? [insideCalculator.finalShapeForPlayer(0), insideCalculator.finalShapeForPlayer(1),
           insideCalculator.finalShapeForPlayer(2)]
        : [0, 0, 0]
    readonly property int distributeTransferCount: (insideCalculator && root.calculationVersion >= 0)
        ? insideCalculator.numberOfDistributeTransfers() : 0

    function distributeTransfersForRound(round) {
        var result = []
        if (!insideCalculator || root.calculationVersion < 0)
            return result
        var total = root.distributeTransferCount
        for (var i = 0; i < total; ++i) {
            if (insideCalculator.distributeTransferRound(i) === round) {
                result.push({
                    from: insideCalculator.distributeTransferFrom(i),
                    to: insideCalculator.distributeTransferTo(i),
                    symbol: insideCalculator.distributeTransferSymbol(i)
                })
            }
        }
        return result
    }

    // Own final pair for the "combine and leave" step. Default: the 2
    // other symbols. Challenge: base pair of the shape called for you.
    readonly property var myFinalPair: {
        if (root.myPosition < 0)
            return []
        var mine = root.playerValue(root.myPosition)
        if (mine <= 0)
            return []
        if (root.challengeMode) {
            var myTarget = root.targetValue(root.myPosition)
            if (myTarget <= 0)
                return []
            var pair = ShapeMath.baseSymbolsFor(myTarget)
            return pair.length === 2 ? pair : []
        }
        return root.symbols2d.filter(function (s) { return s !== mine })
    }

    // Challenge distribute gives: for every other position, how many
    // copies of your own symbol their called shape needs (0-2) - direct
    // hand-off, no wall or engine needed.
    readonly property var myChallengeDistributeGives: {
        var result = []
        if (root.myPosition < 0)
            return result
        var myOwn = root.playerValue(root.myPosition)
        if (myOwn <= 0)
            return result
        for (var i = 0; i < 3; ++i) {
            if (i === root.myPosition)
                continue
            var targetShape = root.targetValue(i)
            if (targetShape <= 0)
                continue
            var pair = ShapeMath.baseSymbolsFor(targetShape)
            var count = pair.filter(function (s) { return s === myOwn }).length
            for (var k = 0; k < count; ++k) {
                result.push({from: root.myPosition, to: i, symbol: myOwn})
            }
        }
        return result
    }

    // Full LFG flow as ONE numbered list of the app user's own actions -
    // no rows for the other 2 players' give/receive traffic.
    readonly property var mySteps: {
        var steps = []
        if (root.myPosition < 0 || !root.myWallComplete)
            return steps
        if (root.challengeMode && (root.target1 <= 0 || root.target2 <= 0 || root.target3 <= 0))
            return steps
        // Disallowed input must not still produce a step list - the local
        // give-computation below doesn't validate anything itself.
        if (root.hasNoSolution)
            return steps

        var n = 0
        root.mySortTransfers.forEach(function (t) {
            n++
            steps.push({type: "give", n: n, from: t.from, to: t.to, symbol: t.symbol})
        })

        steps.push({type: "wait", text: qsTr("Wait until all 3 players have sorted, then distribute")})

        // Both distribute gives back-to-back, no wait between - their
        // order doesn't matter to you. One wait after both, until the 2
        // new symbols show up on your wall.
        var myDistributeGives = root.challengeMode
            ? root.myChallengeDistributeGives
            : root.distributeTransfersForRound(0)
                .concat(root.distributeTransfersForRound(1))
                .filter(function (t) { return t.from === root.myPosition })
        myDistributeGives.forEach(function (t) {
            n++
            steps.push({type: "give", n: n, from: t.from, to: t.to, symbol: t.symbol})
        })

        if (myDistributeGives.length > 0) {
            steps.push({type: "wait", text: qsTr("Wait until the 2 new symbols show up on your wall")})
        }

        if (root.myFinalPair.length === 2) {
            n++
            steps.push({type: "combine", n: n, a: root.myFinalPair[0], b: root.myFinalPair[1]})
        }

        return steps
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
        myPosition = -1
        inferredPlayerIndex = -1
        inferredTargetIndex = -1
        hasNoSolution = false
        insideCalculator.reset()
    }

    function targetValue(idx) {
        return idx === 0 ? target1 : (idx === 1 ? target2 : target3)
    }

    // Copies what's shown in the "Statue positions" row (own 2D symbols) -
    // not the computed final escape shape, which isn't shown there.
    function copyFinalShapes() {
        var lines = []
        for (var i = 0; i < 3; ++i) {
            lines.push(root.playerLabels[i] + ": " + ShapeIcons.shapeName(root.playerValue(i)))
        }
        chatText.text = lines.join("\n")
        chatText.selectAll()
        chatText.copy()
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

    // Clears any already-picked target that's no longer a selectable
    // option after checking "pure shapes only" - avoids a target sitting
    // on a value the grid no longer shows as selectable.
    function sanitizeTargetsForPureOnly() {
        if (!root.pureShapesOnly)
            return
        for (var i = 0; i < 3; ++i) {
            var v = targetValue(i)
            if (v > 0 && root.pureSymbols3d.indexOf(v) === -1) {
                setTargetValue(i, 0)
                if (inferredTargetIndex === i)
                    inferredTargetIndex = -1
            }
        }
        maybeInferMissingTarget()
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

    // calculateSteps() always runs regardless of Challenge Mode - its
    // distribute output isn't used for display (see mySteps /
    // myChallengeDistributeGives, computed locally, no wall needed), it's
    // only kept so stepCount stays a reliable "own symbols valid" signal.
    function tryCalculate() {
        if (player1 <= 0 || player2 <= 0 || player3 <= 0) {
            hasNoSolution = false
            insideCalculator.reset()
            return
        }
        if (!insideCalculator.checkIsValid(player1, player2, player3)) {
            hasNoSolution = true
            insideCalculator.reset()
            return
        }
        insideCalculator.calculateSteps(player1, player2, player3)
        if (root.challengeMode && target1 > 0 && target2 > 0 && target3 > 0) {
            hasNoSolution = !insideCalculator.checkIsValidChallenge(player1, player2, player3,
                                                                      target1, target2, target3)
        } else {
            hasNoSolution = false
        }
    }

    onPlayer1Changed: tryCalculate()
    onPlayer2Changed: tryCalculate()
    onPlayer3Changed: tryCalculate()
    onChallengeModeChanged: tryCalculate()
    onPureShapesOnlyChanged: {
        sanitizeTargetsForPureOnly()
        tryCalculate()
    }

    // What YOU give away this round: any foreign symbol on your wall goes
    // to whoever owns it - only needs their own symbols, not their walls.
    readonly property var mySortTransfers: {
        var result = []
        if (root.myPosition < 0)
            return result
        var own = [root.player1, root.player2, root.player3]
        var myOwn = own[root.myPosition]
        if (myOwn <= 0)
            return result
        var mySlots = [root.wallValue(root.myPosition, 0), root.wallValue(root.myPosition, 1)]
        for (var i = 0; i < 2; ++i) {
            var sym = mySlots[i]
            if (sym > 0 && sym !== myOwn) {
                var owner = own.indexOf(sym)
                if (owner >= 0 && owner !== root.myPosition)
                    result.push({from: root.myPosition, to: owner, symbol: sym})
            }
        }
        return result
    }

    readonly property bool myWallComplete: root.myPosition >= 0
        && root.wallValue(root.myPosition, 0) > 0 && root.wallValue(root.myPosition, 1) > 0

    TextEdit {
        id: chatText
        visible: false
        text: ""
    }

    // Tab bar, narrow layout only - shows one half at a time instead of
    // squeezing both side by side. Matches outsideTabBar's style.
    Rectangle {
        id: insideTabBar
        x: 0
        y: 0
        width: parent.width
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

    Rectangle {
        id: setupFrame
        x: 0
        y: root.wideLayout ? 0 : insideTabBar.height + 8
        width: root.wideLayout ? (root.width - 12) * 0.42 : root.width
        height: root.height - y
        opacity: (root.wideLayout || root.tab === 0) ? 1 : 0
        visible: opacity > 0
        enabled: root.wideLayout || root.tab === 0
        Behavior on opacity { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
        color: "#0d0d0d"
        radius: 10
        border.color: "#2a2a2a"

        Column {
            id: leftCol
            x: 16
            y: 16
            width: parent.width - 32
            height: parent.height - 32
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

            // Category 1: own symbols + wall - everything for sort phase.
            Rectangle {
                width: parent.width
                height: symbolsCategory.implicitHeight + 28
                radius: 8
                color: "#141414"
                border.color: "#2a2a2a"

                Column {
                    id: symbolsCategory
                    x: 14
                    y: 14
                    width: parent.width - 28
                    spacing: 14

                    Text {
                        text: qsTr("Solo room symbols")
                        color: "#ffffff"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    Text {
                        text: qsTr("Each player's own statue symbol - your own is required, teammates' are only needed so the give/take instructions can name the right person")
                        color: "#999999"
                        font.pixelSize: 11
                        wrapMode: Text.WordWrap
                        width: parent.width
                    }

                    Row {
                        spacing: 14

                        Text {
                            text: qsTr("Which statue are you?")
                            color: "#cccccc"
                            font.pixelSize: 13
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Row {
                            spacing: 8
                            anchors.verticalCenter: parent.verticalCenter

                            Repeater {
                                model: 3

                                delegate: Rectangle {
                                    id: posButton
                                    required property int index
                                    width: 56
                                    height: 28
                                    radius: 6
                                    color: root.myPosition === posButton.index ? "#3b82f6" : "#2a2a2a"
                                    border.color: "#444444"

                                    Text {
                                        anchors.centerIn: parent
                                        text: root.playerLabels[posButton.index]
                                        color: "#ffffff"
                                        font.pixelSize: 12
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: root.myPosition = (root.myPosition === posButton.index ? -1 : posButton.index)
                                    }
                                }
                            }
                        }
                    }

                    Text {
                        text: qsTr("Which statue shows which symbol?")
                        color: "#cccccc"
                        font.pixelSize: 13
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
                                readonly property bool isMe: playerCol.index === root.myPosition
                                width: (playerRow.width - playerRow.spacing * 2) / 3
                                spacing: 6

                                Text {
                                    text: playerCol.isMe ? qsTr("%1 (You)").arg(root.playerLabels[playerCol.index])
                                                          : root.playerLabels[playerCol.index]
                                    color: playerCol.isMe ? "#7fb2ff" : "#999999"
                                    font.bold: playerCol.isMe
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

                    Rectangle {
                        width: parent.width
                        height: 1
                        color: "#262626"
                    }

                    Text {
                        text: qsTr("Sort: give away any wall symbol that isn't your own to whoever owns it, wait for teammates, then distribute.")
                        color: "#999999"
                        font.pixelSize: 11
                        wrapMode: Text.WordWrap
                        width: parent.width
                    }

                    Text {
                        text: qsTr("Your wall right now")
                        color: "#cccccc"
                        font.pixelSize: 13
                        visible: root.myPosition >= 0
                    }

                    Text {
                        text: root.myPosition >= 0
                              ? qsTr("2 symbols - tap a 2nd symbol to complete the pair, or double-tap one symbol for 2 of the same")
                              : qsTr("Pick which statue you are above to enter your wall")
                        color: "#999999"
                        font.pixelSize: 11
                        wrapMode: Text.WordWrap
                        width: parent.width
                    }

                    // Only own wall shown. Positioned by explicit x, not a
                    // Row of 3 columns - Row excludes invisible children
                    // from layout, which collapsed everything to the left
                    // when only 1 of 3 columns was ever visible. Same
                    // per-column width/spacing as the symbol row above so
                    // it still lines up under the matching column.
                    Item {
                        id: wallAlignArea
                        width: parent.width
                        height: wallColumnContent.implicitHeight
                        visible: root.myPosition >= 0

                        readonly property real columnSpacing: 12
                        readonly property real colWidth: (width - columnSpacing * 2) / 3

                        Column {
                            id: wallColumnContent
                            x: root.myPosition >= 0 ? root.myPosition * (wallAlignArea.colWidth + wallAlignArea.columnSpacing) : 0
                            width: wallAlignArea.colWidth
                            spacing: 4

                            Text {
                                text: root.myPosition >= 0 ? qsTr("%1 (You)").arg(root.playerLabels[root.myPosition]) : ""
                                color: "#7fb2ff"
                                font.bold: true
                                font.pixelSize: 11
                                anchors.horizontalCenter: parent.horizontalCenter
                            }

                            WallPairSelector {
                                totalWidth: wallAlignArea.colWidth
                                cellSpacing: 4
                                options: root.symbols2d
                                slotA: root.myPosition >= 0 ? root.wallValue(root.myPosition, 0) : 0
                                slotB: root.myPosition >= 0 ? root.wallValue(root.myPosition, 1) : 0
                                onPairChanged: (a, b) => {
                                    if (root.myPosition >= 0) {
                                        root.setWallValue(root.myPosition, 0, a)
                                        root.setWallValue(root.myPosition, 1, b)
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Category 2: Challenge Mode - own card, only affects distribute.
            Rectangle {
                width: parent.width
                height: challengeCategory.implicitHeight + 28
                radius: 8
                color: "#141414"
                border.color: "#2a2a2a"
                visible: root.encounterProgress !== null && root.encounterProgress !== undefined

                Column {
                    id: challengeCategory
                    x: 14
                    y: 14
                    width: parent.width - 28
                    spacing: 14

                    Row {
                        spacing: 14

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

                        Row {
                            spacing: 8
                            anchors.verticalCenter: parent.verticalCenter

                            Rectangle {
                                id: pureOnlyBox
                                width: 18
                                height: 18
                                radius: 4
                                anchors.verticalCenter: parent.verticalCenter
                                color: root.pureShapesOnly ? "#3b82f6" : "#2a2a2a"
                                border.color: "#555555"

                                Text {
                                    anchors.centerIn: parent
                                    text: "✓"
                                    visible: root.pureShapesOnly
                                    color: "#ffffff"
                                    font.bold: true
                                    font.pixelSize: 12
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: root.pureShapesOnly = !root.pureShapesOnly
                                }
                            }

                            Text {
                                text: qsTr("Pure shapes only")
                                color: "#cccccc"
                                font.pixelSize: 13
                                anchors.verticalCenter: parent.verticalCenter

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: root.pureShapesOnly = !root.pureShapesOnly
                                }
                            }
                        }
                    }

                    Text {
                        visible: root.challengeMode
                        text: qsTr("Default distribute always builds a mixed shape - Challenge Mode only needs to cover the 3 pure ones (Pyramide/Wuerfel/Kugel). Uncheck to pick any of the 6 instead.")
                        color: "#777777"
                        font.pixelSize: 10
                        wrapMode: Text.WordWrap
                        width: parent.width
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
                                        options: root.targetShapeOptions
                                        selected: targetCol.index === 0 ? root.target1
                                                                         : (targetCol.index === 1 ? root.target2 : root.target3)
                                        selectionIsInferred: targetCol.index === root.inferredTargetIndex
                                        onTapped: (value) => root.setTarget(targetCol.index, value)
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Column {
                width: parent.width
                spacing: 8

                Text {
                    text: qsTr("Note: the triangle/square/circle row further up (2 symbols) is a separate pick from your own symbol above it - it's what's currently on your statue's wall, not necessarily your own symbol.")
                    color: "#999999"
                    font.pixelSize: 11
                    wrapMode: Text.WordWrap
                    width: parent.width
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
    }

    Rectangle {
        id: solutionFrame
        x: root.wideLayout ? setupFrame.width + 12 : 0
        y: root.wideLayout ? 0 : insideTabBar.height + 8
        width: root.wideLayout ? (root.width - 12) * 0.58 : root.width
        height: root.height - y
        opacity: (root.wideLayout || root.tab === 1) ? 1 : 0
        visible: opacity > 0
        enabled: root.wideLayout || root.tab === 1
        Behavior on opacity { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
        color: "#0d0d0d"
        radius: 10
        border.color: "#2a2a2a"

        Column {
            id: rightCol
            x: 16
            y: 16
            width: parent.width - 32
            height: parent.height - 32
            spacing: 14

            Text {
                id: escapeTitle
                text: qsTr("Solution")
                color: "#ffffff"
                font.pixelSize: 18
                font.bold: true
            }

            Row {
                width: parent.width
                spacing: 10

                Text {
                    text: qsTr("Statue positions")
                    color: "#999999"
                    font.pixelSize: 11
                    anchors.verticalCenter: parent.verticalCenter
                }

                // Copies the statue/symbol assignments, not the step list -
                // what the outside team needs called out in chat.
                Rectangle {
                    id: copyButton
                    width: 28
                    height: 22
                    radius: 5
                    color: "#2a2a2a"
                    border.color: copyButton.justCopied ? "#2e7d46" : "#444444"
                    anchors.verticalCenter: parent.verticalCenter

                    property bool justCopied: false

                    Behavior on border.color { ColorAnimation { duration: 120 } }

                    // Brief "copied" confirmation - resets itself, no need
                    // to track cancellation since a 2nd click just restarts
                    // the same timer.
                    Timer {
                        id: copiedResetTimer
                        interval: 1100
                        onTriggered: copyButton.justCopied = false
                    }

                    // Simple 2-square "copy" glyph, drawn manually instead
                    // of relying on a clipboard emoji being in the font -
                    // swapped for a checkmark right after copying.
                    Rectangle {
                        visible: !copyButton.justCopied
                        x: 7
                        y: 5
                        width: 9
                        height: 9
                        radius: 2
                        color: "transparent"
                        border.color: "#dddddd"
                        border.width: 1.3
                    }
                    Rectangle {
                        visible: !copyButton.justCopied
                        x: 10
                        y: 8
                        width: 9
                        height: 9
                        radius: 2
                        color: "#2a2a2a"
                        border.color: "#dddddd"
                        border.width: 1.3
                    }
                    Text {
                        visible: copyButton.justCopied
                        anchors.centerIn: parent
                        text: "✓"
                        color: "#2e7d46"
                        font.bold: true
                        font.pixelSize: 14
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            root.copyFinalShapes()
                            copyButton.justCopied = true
                            copiedResetTimer.restart()
                        }
                    }
                }
            }

            Row {
                id: shapesRow
                spacing: 14

                Repeater {
                    model: 3

                    delegate: Column {
                        id: shapeEntry
                        required property int index
                        readonly property bool isMe: shapeEntry.index === root.myPosition
                        spacing: 4

                        Text {
                            text: shapeEntry.isMe ? qsTr("%1 (You)").arg(root.playerLabels[shapeEntry.index])
                                                   : root.playerLabels[shapeEntry.index]
                            color: shapeEntry.isMe ? "#7fb2ff" : "#999999"
                            font.bold: shapeEntry.isMe
                            font.pixelSize: 10
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Rectangle {
                            width: 60
                            height: 60
                            radius: 6
                            color: "#161616"
                            border.color: shapeEntry.isMe ? "#3b82f6" : "#333333"
                            border.width: shapeEntry.isMe ? 2 : 1

                            // Seeded from own-symbol picks, not the final
                            // escape shape (already shown by "Combine" below).
                            Image {
                                anchors.centerIn: parent
                                width: 40
                                height: 40
                                fillMode: Image.PreserveAspectFit
                                source: ShapeIcons.iconSource(root.playerValue(shapeEntry.index))
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

                    // Own steps only, flat numbered list (matches outside's
                    // StepCard list) - no rows for the other 2 players.
                    Column {
                        width: stepsColumn.width
                        spacing: 10

                        Text {
                            visible: root.myWallComplete && root.mySteps.length === 0
                            text: qsTr("Nothing to give - you already have 2 of your own symbol")
                            color: "#888888"
                            font.pixelSize: 11
                        }

                        Repeater {
                            model: root.mySteps

                            delegate: Item {
                                id: stepEntry
                                required property var modelData
                                width: stepsColumn.width
                                height: actionCard.visible ? actionCard.height : waitBanner.height

                                TransferStepCard {
                                    id: actionCard
                                    visible: stepEntry.modelData.type !== "wait"
                                    width: parent.width
                                    stepNumber: stepEntry.modelData.n
                                    mode: stepEntry.modelData.type === "combine" ? "combine" : "give"
                                    fromLabel: stepEntry.modelData.type === "give" ? root.playerLabels[stepEntry.modelData.from] : ""
                                    toLabel: stepEntry.modelData.type === "give" ? root.playerLabels[stepEntry.modelData.to] : ""
                                    symbol: stepEntry.modelData.type === "give" ? stepEntry.modelData.symbol : 0
                                    symbolA: stepEntry.modelData.type === "combine" ? stepEntry.modelData.a : 0
                                    symbolB: stepEntry.modelData.type === "combine" ? stepEntry.modelData.b : 0
                                }

                                Rectangle {
                                    id: waitBanner
                                    visible: stepEntry.modelData.type === "wait"
                                    width: parent.width
                                    height: Math.max(40, waitText.implicitHeight + 20)
                                    radius: 6
                                    color: "#1a2a3a"
                                    border.color: "#3b5a7a"

                                    Text {
                                        id: waitText
                                        anchors.centerIn: parent
                                        width: parent.width - 24
                                        horizontalAlignment: Text.AlignHCenter
                                        wrapMode: Text.WordWrap
                                        text: stepEntry.modelData.text
                                        color: "#9dc4e8"
                                        font.pixelSize: 14
                                    }
                                }
                            }
                        }
                    }

                    Text {
                        visible: !root.hasNoSolution && root.stepCount === 0
                        text: qsTr("Select each player's own symbol to see the solution.")
                        color: "#666666"
                        font.pixelSize: 12
                    }

                    Text {
                        visible: !root.hasNoSolution && root.stepCount > 0 && !root.myWallComplete
                        text: qsTr("Enter your wall above to see the sort and distribute steps.")
                        color: "#666666"
                        font.pixelSize: 12
                    }

                    // Last item in the scrollable list, not a fixed footer
                    // below it - a footer could end up pushed off-screen on
                    // short windows. Narrow layout only.
                    Rectangle {
                        width: 90
                        height: 34
                        radius: 6
                        color: "#2a2a2a"
                        border.color: "#444444"
                        visible: !root.wideLayout

                        Text {
                            anchors.centerIn: parent
                            text: qsTr("Reset")
                            color: "#dddddd"
                            font.pixelSize: 13
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                root.reset()
                                root.tab = 0
                            }
                        }
                    }
                }
            }
        }
    }
}
