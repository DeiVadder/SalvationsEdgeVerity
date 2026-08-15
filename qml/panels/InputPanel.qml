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
    readonly property var symbols3d: [Symbols.Kegel, Symbols.Wuerfel, Symbols.Zylinder,
        Symbols.Pyramide, Symbols.Prisma, Symbols.Kugel]

    property int inner1: 0
    property int inner2: 0
    property int inner3: 0
    property int outer1: 0
    property int outer2: 0
    property int outer3: 0

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

    function reset() {
        inner1 = 0; inner2 = 0; inner3 = 0
        outer1 = 0; outer2 = 0; outer3 = 0
        inferredInnerIndex = -1
        inferredOuterIndex = -1
        hasNoSolution = false
        stepCalculator.reset()
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
        stepCalculator.calculateSteps(inner1, inner2, inner3, outer1, outer2, outer3)
        hasNoSolution = !stepCalculator.isSolved()
    }

    onInner1Changed: tryCalculate()
    onInner2Changed: tryCalculate()
    onInner3Changed: tryCalculate()
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
