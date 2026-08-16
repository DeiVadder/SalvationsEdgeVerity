import QtQuick
import CalculateSteps 1.0
import SymbolEnums 1.0
import "../js/ShapeMath.js" as ShapeMath

// Outside-puzzle input: per statue (LEFT/MID/RIGHT), pick the inside 2D
// symbol the corresponding solo player needs and the statue's current
// outside 3D shape. Recomputes automatically whenever a selection changes.
Rectangle {
    id: root

    property CalculateSteps stepCalculator
    property var encounterProgress
    readonly property var statueLabels: [qsTr("LEFT"), qsTr("MID"), qsTr("RIGHT")]
    readonly property var symbols2d: [Symbols.Dreieck, Symbols.Viereck, Symbols.Kreis]
    // Row 2 (Pyramide/Wuerfel/Kugel) are the 3 "pure double" shapes,
    // ordered to match symbols2d's Dreieck/Viereck/Kreis columns above -
    // row 1 holds the remaining 3 mixed-pair shapes.
    readonly property var symbols3d: [Symbols.Kegel, Symbols.Zylinder, Symbols.Prisma,
        Symbols.Pyramide, Symbols.Wuerfel, Symbols.Kugel]
    readonly property var pureSymbols3d: [Symbols.Pyramide, Symbols.Wuerfel, Symbols.Kugel]

    // Default distribution always targets fromBaseSymbol(inner) - one of
    // the 3 MIXED shapes, since that formula never includes the statue's
    // own inner symbol. Challenge Mode only ever needs to exist to reach
    // one of the 3 PURE shapes instead (default can never produce those) -
    // see calculatesteps.h's calculateStepsChallenge() doc comment. On by
    // default; uncheck for the full 6-shape picker if a real call ever
    // needs to reassign a mixed shape to a different statue instead.
    property bool pureShapesOnly: true
    readonly property var targetShapeOptions: root.pureShapesOnly ? root.pureSymbols3d : root.symbols3d

    property int inner1: 0
    property int inner2: 0
    property int inner3: 0
    property int outer1: 0
    property int outer2: 0
    property int outer3: 0

    // Challenge Mode only - the outside team's called escape shape per
    // statue, overriding the default fromBaseSymbol(inner) target.
    property int target1: 0
    property int target2: 0
    property int target3: 0
    property int inferredTargetIndex: -1

    // Once 2 of the 3 inside symbols are picked (and distinct), the 3rd is
    // forced - the game always shows 3 pairwise-distinct callouts. Track
    // which slot (if any) currently holds that inferred guess so it can be
    // highlighted and still overridden by the user.
    property int inferredInnerIndex: -1
    // Each base 2D symbol appears exactly twice across the 3 outside 3D
    // shapes (checkIsValid() enforces this). Once 2 of the 3 shapes are
    // picked, the base-symbol pair needed for the 3rd is always forced.
    property int inferredOuterIndex: -1
    property bool hasNoSolution: false

    signal invalidInput()

    readonly property bool challengeMode: root.encounterProgress
        ? root.encounterProgress.challengeModeEnabled : false

    function reset() {
        inner1 = 0; inner2 = 0; inner3 = 0
        outer1 = 0; outer2 = 0; outer3 = 0
        target1 = 0; target2 = 0; target3 = 0
        inferredInnerIndex = -1
        inferredOuterIndex = -1
        inferredTargetIndex = -1
        hasNoSolution = false
        stepCalculator.reset()
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

    // Clears any already-picked target that's no longer a selectable
    // option after checking "pure shapes only".
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

    function outerValue(idx) {
        return idx === 0 ? outer1 : (idx === 1 ? outer2 : outer3)
    }

    function setOuterValue(idx, value) {
        if (idx === 0) outer1 = value
        else if (idx === 1) outer2 = value
        else outer3 = value
    }

    function setOuter(idx, value) {
        if (idx === inferredOuterIndex) {
            inferredOuterIndex = -1
        } else if (inferredOuterIndex >= 0) {
            // A manual tap on one of the other two slots invalidates the
            // current guess - clear it so the stale value can't linger.
            setOuterValue(inferredOuterIndex, 0)
            inferredOuterIndex = -1
        }
        setOuterValue(idx, value)
        maybeInferMissingOuter()
    }

    function maybeInferMissingOuter() {
        var vals = [outer1, outer2, outer3]
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
                    setOuterValue(zeroIdx, inferredShape)
                    inferredOuterIndex = zeroIdx
                }
            }
        } else if (setIdx.length < 2) {
            inferredOuterIndex = -1
        }
    }

    function innerValue(idx) {
        return idx === 0 ? inner1 : (idx === 1 ? inner2 : inner3)
    }

    function setInnerValue(idx, value) {
        if (idx === 0) inner1 = value
        else if (idx === 1) inner2 = value
        else inner3 = value
    }

    function setInner(idx, value) {
        // Tapping the slot that was previously a guess makes it manual.
        if (idx === inferredInnerIndex)
            inferredInnerIndex = -1
        // If another slot is still an inferred guess and this tap now
        // duplicates it, that guess is stale - clear it back out.
        if (inferredInnerIndex >= 0 && innerValue(inferredInnerIndex) === value) {
            setInnerValue(inferredInnerIndex, 0)
            inferredInnerIndex = -1
        }
        setInnerValue(idx, value)
        maybeInferMissingInner()
    }

    function maybeInferMissingInner() {
        var vals = [inner1, inner2, inner3]
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
                setInnerValue(zeroIdx, remaining[0])
                inferredInnerIndex = zeroIdx
            }
        } else if (setIdx.length < 2) {
            inferredInnerIndex = -1
        }
    }

    function tryCalculate() {
        if (inner1 <= 0 || inner2 <= 0 || inner3 <= 0
                || outer1 <= 0 || outer2 <= 0 || outer3 <= 0) {
            hasNoSolution = false
            stepCalculator.reset()
            return
        }
        if (!stepCalculator.checkIsValid(inner1, inner2, inner3, outer1, outer2, outer3)) {
            hasNoSolution = true
            stepCalculator.reset()
            invalidInput()
            return
        }

        if (root.challengeMode) {
            if (target1 <= 0 || target2 <= 0 || target3 <= 0) {
                hasNoSolution = false
                stepCalculator.reset()
                return
            }
            if (!stepCalculator.checkIsValidChallenge(inner1, inner2, inner3, target1, target2, target3)) {
                hasNoSolution = true
                stepCalculator.reset()
                invalidInput()
                return
            }
            stepCalculator.calculateStepsChallenge(outer1, outer2, outer3, target1, target2, target3)
            hasNoSolution = !stepCalculator.isSolved()
            return
        }

        stepCalculator.calculateSteps(inner1, inner2, inner3, outer1, outer2, outer3)
        hasNoSolution = !stepCalculator.isSolved()
    }

    onInner1Changed: tryCalculate()
    onInner2Changed: tryCalculate()
    onInner3Changed: tryCalculate()
    onChallengeModeChanged: tryCalculate()
    onPureShapesOnlyChanged: {
        sanitizeTargetsForPureOnly()
        tryCalculate()
    }
    onOuter1Changed: tryCalculate()
    onOuter2Changed: tryCalculate()
    onOuter3Changed: tryCalculate()

    color: "#0d0d0d"
    radius: 10
    border.color: "#2a2a2a"

    // Whole-panel Flickable, not just the fill-anchored Column it used to
    // be - a fixed-height Column silently clips/overflows when the panel
    // is given less height than its content needs (narrow/short windows),
    // with no way to scroll down to whatever got cut off.
    Flickable {
        anchors.fill: parent
        contentWidth: width
        contentHeight: mainColumn.implicitHeight + 32
        clip: true
        boundsBehavior: Flickable.StopAtBounds

    Column {
        id: mainColumn
        x: 16
        y: 16
        width: parent.width - 32
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
            text: qsTr("Select all inside 2D shapes")
            color: "#ffffff"
            font.pixelSize: 15
            font.bold: true
        }

        Row {
            id: innerRow
            width: parent.width
            spacing: 12

            Repeater {
                model: 3

                delegate: Column {
                    id: innerCol
                    required property int index
                    width: (innerRow.width - innerRow.spacing * 2) / 3
                    spacing: 6

                    Text {
                        text: root.statueLabels[innerCol.index]
                        color: "#999999"
                        font.pixelSize: 11
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    ShapeGridSelector {
                        columns: 3
                        totalWidth: innerCol.width
                        cellSpacing: 6
                        options: root.symbols2d
                        selected: innerCol.index === 0 ? root.inner1
                                                        : (innerCol.index === 1 ? root.inner2 : root.inner3)
                        selectionIsInferred: innerCol.index === root.inferredInnerIndex
                        onTapped: (value) => root.setInner(innerCol.index, value)
                    }
                }
            }
        }

        Text {
            text: qsTr("Select all outside 3D shapes")
            color: "#ffffff"
            font.pixelSize: 15
            font.bold: true
        }

        Row {
            id: outerRow
            width: parent.width
            spacing: 12

            Repeater {
                model: 3

                delegate: Column {
                    id: outerCol
                    required property int index
                    width: (outerRow.width - outerRow.spacing * 2) / 3
                    spacing: 6

                    Text {
                        text: root.statueLabels[outerCol.index]
                        color: "#999999"
                        font.pixelSize: 11
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    ShapeGridSelector {
                        columns: 3
                        totalWidth: outerCol.width
                        cellSpacing: 6
                        options: root.symbols3d
                        selected: outerCol.index === 0 ? root.outer1
                                                        : (outerCol.index === 1 ? root.outer2 : root.outer3)
                        selectionIsInferred: outerCol.index === root.inferredOuterIndex
                        onTapped: (value) => root.setOuter(outerCol.index, value)
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
                color: root.encounterProgress && root.encounterProgress.challengeModeEnabled ? "#3b82f6" : "#333333"

                Rectangle {
                    width: 18
                    height: 18
                    radius: 9
                    color: "white"
                    y: 2
                    x: root.encounterProgress && root.encounterProgress.challengeModeEnabled ? parent.width - width - 2 : 2
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
            text: qsTr("Default distribution always builds a mixed shape - Challenge Mode only needs to cover the 3 pure ones (Pyramide/Wuerfel/Kugel). Uncheck to pick any of the 6 instead.")
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
                            text: root.statueLabels[targetCol.index]
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
}
