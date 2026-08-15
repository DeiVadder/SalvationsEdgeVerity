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

    // What's actually on each player's wall right now (2 symbols each) -
    // always required, drives the real LFG cleanse/distribute sequence
    // (default) or the experimental Fast shortcut. UNVERIFIED AGAINST REAL
    // GAMEPLAY - see calculateinsidesteps.h and fastcleanseresolver.h.
    property string cleanseMethod: "lfg" // "lfg" (default) | "fast" (experimental)
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
        myPosition = -1
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

    // Distribute-phase math only ever depends on the 3 players' own
    // symbols (fromBaseSymbol(own) - see calculateinsidesteps.cpp), never
    // on wall contents, so it can always be computed once all 3 own
    // symbols are picked. Sort-phase (what YOU give away) is computed
    // separately below from your own wall alone - see mySortTransfers.
    // LFG/Fast/Challenge Mode selection is hidden for now (see mySortTransfers
    // and the hidden UI blocks below) - restore the old wall-based
    // multi-player calculateStepsLFG()/Challenge()/Fast() flow here if that
    // comes back.
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
        hasNoSolution = false
    }

    onPlayer1Changed: tryCalculate()
    onPlayer2Changed: tryCalculate()
    onPlayer3Changed: tryCalculate()

    // What YOU personally give away this round: any foreign symbol on your
    // own wall goes to whichever of the 3 players owns it - the only
    // teammate data this needs is their OWN symbols (already picked above),
    // not their wall contents, so teammates' walls are no longer collected
    // in the UI. Matches the 2 confirmed special cases: wall already shows
    // 2x your own symbol -> 0 gives; wall shows 0x your own symbol -> 2 gives.
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

            // Challenge Mode / outside-escape-shape targets - hidden for
            // now (comes later), state kept so it's a 1-line revert.
            Row {
                spacing: 14
                visible: false

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
                visible: false

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

            Column {
                width: parent.width
                spacing: 8

                Text {
                    text: qsTr("UNVERIFIED against real gameplay - sanity-check in a live run before trusting this")
                    color: "#e0a030"
                    font.pixelSize: 11
                    wrapMode: Text.WordWrap
                    width: parent.width
                }

                Text {
                    text: qsTr("Sort: give away any wall symbol that isn't your own to whoever owns it, wait for teammates, then distribute.")
                    color: "#999999"
                    font.pixelSize: 11
                    wrapMode: Text.WordWrap
                    width: parent.width
                }

                // LFG/Fast method choice - hidden for now (comes later),
                // sort is always computed the LFG way in the meantime.
                Row {
                    spacing: 10
                    visible: false

                    Repeater {
                        model: [{key: "lfg", label: qsTr("LFG")}, {key: "fast", label: qsTr("Fast (Experimental)")}]

                        delegate: Rectangle {
                            id: methodButton
                            required property var modelData
                            width: methodLabel.implicitWidth + 20
                            height: 28
                            radius: 6
                            color: root.cleanseMethod === modelData.key ? "#3b82f6" : "#2a2a2a"
                            border.color: "#444444"

                            Text {
                                id: methodLabel
                                anchors.centerIn: parent
                                text: methodButton.modelData.label
                                color: "#ffffff"
                                font.pixelSize: 12
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: root.cleanseMethod = methodButton.modelData.key
                            }
                        }
                    }
                }

                Text {
                    text: root.myPosition >= 0
                          ? qsTr("Your wall right now (2 symbols) - tap a 2nd symbol to complete the pair, or double-tap one symbol for 2 of the same")
                          : qsTr("Pick which statue you are above to enter your wall")
                    color: "#999999"
                    font.pixelSize: 11
                    wrapMode: Text.WordWrap
                    width: parent.width
                }

                // Only your own wall is needed (see mySortTransfers above) -
                // teammates' walls would just be noise here.
                Column {
                    width: parent.width
                    spacing: 4
                    visible: root.myPosition >= 0

                    WallPairSelector {
                        totalWidth: Math.min(parent.width, 220)
                        cellSpacing: 4
                        options: root.symbols2d
                        slotA: root.myPosition >= 0 ? root.wallValue(root.myPosition, 0) : 0
                        slotB: root.myPosition >= 0 ? root.wallValue(root.myPosition, 1) : 0
                        onPairChanged: (a, b) => {
                            root.setWallValue(root.myPosition, 0, a)
                            root.setWallValue(root.myPosition, 1, b)
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
                text: qsTr("Solution")
                color: "#ffffff"
                font.pixelSize: 18
                font.bold: true
            }

            Text {
                text: qsTr("Statue positions")
                color: "#999999"
                font.pixelSize: 11
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

                            // Seeded live from the own-symbol picks on the
                            // left (not the computed final escape shape -
                            // that's redundant with each StepCard's own
                            // expected-state footer further down).
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

                    // LFG (default): sort phase, sync callout, then distribute phase.
                    Column {
                        width: stepsColumn.width
                        spacing: 10
                        visible: root.cleanseMethod === "lfg"

                        Text {
                            visible: root.mySortTransfers.length > 0
                            text: qsTr("SORT PHASE")
                            color: "#888888"
                            font.pixelSize: 10
                            font.letterSpacing: 1
                        }

                        Text {
                            visible: root.myWallComplete && root.mySortTransfers.length === 0
                            text: qsTr("Nothing to give - you already have 2 of your own symbol")
                            color: "#888888"
                            font.pixelSize: 11
                        }

                        Repeater {
                            model: root.cleanseMethod === "lfg" ? root.mySortTransfers : []

                            delegate: Rectangle {
                                id: sortRow
                                required property var modelData
                                readonly property bool involvesMe: root.myPosition >= 0
                                    && (modelData.from === root.myPosition || modelData.to === root.myPosition)
                                width: stepsColumn.width
                                height: 34
                                radius: 6
                                color: sortRow.involvesMe ? "#1e3a5f" : "#161616"
                                border.color: sortRow.involvesMe ? "#3b82f6" : "#333333"

                                Row {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                    spacing: 8

                                    Text {
                                        text: qsTr("%1 gives").arg(root.playerLabels[sortRow.modelData.from])
                                        color: "#dddddd"
                                        font.pixelSize: 12
                                    }

                                    Image {
                                        width: 20
                                        height: 20
                                        fillMode: Image.PreserveAspectFit
                                        source: ShapeIcons.iconSource(sortRow.modelData.symbol)
                                    }

                                    Text {
                                        text: qsTr("to %1").arg(root.playerLabels[sortRow.modelData.to])
                                        color: "#dddddd"
                                        font.pixelSize: 12
                                    }
                                }
                            }
                        }

                        Rectangle {
                            visible: root.myWallComplete
                            width: stepsColumn.width
                            height: 32
                            radius: 6
                            color: "#1a2a3a"
                            border.color: "#3b5a7a"

                            Text {
                                anchors.centerIn: parent
                                text: qsTr("Wait until all 3 players have sorted, then distribute")
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
                            model: root.cleanseMethod === "lfg" ? root.stepCount : 0

                            delegate: StepCard {
                                id: distributeCard
                                required property int index
                                width: stepsColumn.width
                                stepNumber: distributeCard.index + 1
                                nodeLabels: root.playerLabels
                                highlightIndex: root.myPosition
                                instructions: root.calculationVersion >= 0 ? [
                                    root.insideCalculator.getInstructionForStep(distributeCard.index, 0),
                                    root.insideCalculator.getInstructionForStep(distributeCard.index, 1),
                                    root.insideCalculator.getInstructionForStep(distributeCard.index, 2)
                                ] : [0, 0, 0]
                                expectedState: root.finalShapes
                            }
                        }
                    }

                    // Fast (experimental): local decision-table rounds, no sync callout.
                    Column {
                        width: stepsColumn.width
                        spacing: 10
                        visible: root.cleanseMethod === "fast"

                        Repeater {
                            model: root.cleanseMethod === "fast" ? root.fastRoundCount : 0

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
                                        id: transferRow
                                        required property var modelData
                                        readonly property bool involvesMe: root.myPosition >= 0
                                            && (modelData.from === root.myPosition || modelData.to === root.myPosition)
                                        width: stepsColumn.width
                                        height: 34
                                        radius: 6
                                        color: transferRow.involvesMe ? "#1e3a5f" : "#161616"
                                        border.color: transferRow.involvesMe ? "#3b82f6" : "#333333"

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
                        text: qsTr("Select each player's own symbol to see the solution.")
                        color: "#666666"
                        font.pixelSize: 12
                    }
                }
            }
        }
}
