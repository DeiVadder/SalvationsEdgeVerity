import QtQuick
import CalculateSteps 1.0
import SymbolEnums 1.0

// Outside-puzzle input: per statue (LEFT/MID/RIGHT), pick the inside 2D
// symbol the corresponding solo player needs and the statue's current
// outside 3D shape. Recomputes automatically whenever a selection changes.
Rectangle {
    id: root

    property CalculateSteps stepCalculator
    property var encounterProgress
    readonly property var statueLabels: ["LEFT", "MID", "RIGHT"]
    readonly property var symbols2d: [Symbols.Dreieck, Symbols.Viereck, Symbols.Kreis]
    readonly property var symbols3d: [Symbols.Kegel, Symbols.Wuerfel, Symbols.Zylinder,
        Symbols.Pyramide, Symbols.Prisma, Symbols.Kugel]

    property int inner1: 0
    property int inner2: 0
    property int inner3: 0
    property int outer1: 0
    property int outer2: 0
    property int outer3: 0

    signal invalidInput()

    function reset() {
        inner1 = 0; inner2 = 0; inner3 = 0
        outer1 = 0; outer2 = 0; outer3 = 0
        stepCalculator.reset()
    }

    function tryCalculate() {
        if (inner1 <= 0 || inner2 <= 0 || inner3 <= 0
                || outer1 <= 0 || outer2 <= 0 || outer3 <= 0) {
            stepCalculator.reset()
            return
        }
        if (!stepCalculator.checkIsValid(inner1, inner2, inner3, outer1, outer2, outer3)) {
            stepCalculator.reset()
            invalidInput()
            return
        }
        stepCalculator.calculateSteps(inner1, inner2, inner3, outer1, outer2, outer3)
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

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 18

        Text {
            text: "Select all inside 2D shapes"
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
                        onTapped: (value) => {
                            if (innerCol.index === 0) root.inner1 = value
                            else if (innerCol.index === 1) root.inner2 = value
                            else root.inner3 = value
                        }
                    }
                }
            }
        }

        Text {
            text: "Select all outside 3D shapes"
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
                        onTapped: (value) => {
                            if (outerCol.index === 0) root.outer1 = value
                            else if (outerCol.index === 1) root.outer2 = value
                            else root.outer3 = value
                        }
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
}
