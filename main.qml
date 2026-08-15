import QtQuick
import QtQuick.Window
import QtQuick.Controls

import CalculateSteps 1.0

import "qml/panels"
import "qml/challenge"
import "qml/ghost"

ApplicationWindow {
    id: root
    width: 900
    height: 640
    visible: true
    title: qsTr("Salvation's Edge Verity Encounter")
    color: "#050505"

    // 0 = outside dissection puzzle, 1 = inside/solo-room puzzle, 2 = ghost phase helper
    property int puzzleMode: 0
    readonly property bool wideLayout: width >= height * 1.15

    CalculateSteps {
        id: stepCalculator
    }

    CalculateInsideSteps {
        id: insideStepCalculator
    }

    EncounterProgress {
        id: encounterProgress
    }

    GhostPhaseHelper {
        id: ghostHelper
    }

    Column {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 12

        Row {
            width: parent.width
            height: 40
            spacing: 12

            Text {
                text: root.title
                color: "#ffffff"
                font.pixelSize: 18
                font.bold: true
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width - modeToggle.width - resetButton.width - 24
                elide: Text.ElideRight
            }

            Rectangle {
                id: modeToggle
                width: 320
                height: 34
                radius: 6
                color: "#161616"
                border.color: "#333333"
                anchors.verticalCenter: parent.verticalCenter

                Row {
                    anchors.fill: parent
                    anchors.margins: 2

                    Rectangle {
                        width: parent.width / 3
                        height: parent.height
                        radius: 5
                        color: root.puzzleMode === 0 ? "#3b82f6" : "transparent"
                        Text {
                            anchors.centerIn: parent
                            text: "Outside"
                            color: root.puzzleMode === 0 ? "white" : "#999999"
                            font.pixelSize: 12
                            font.bold: root.puzzleMode === 0
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: root.puzzleMode = 0
                        }
                    }
                    Rectangle {
                        width: parent.width / 3
                        height: parent.height
                        radius: 5
                        color: root.puzzleMode === 1 ? "#3b82f6" : "transparent"
                        Text {
                            anchors.centerIn: parent
                            text: "Inside"
                            color: root.puzzleMode === 1 ? "white" : "#999999"
                            font.pixelSize: 12
                            font.bold: root.puzzleMode === 1
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: root.puzzleMode = 1
                        }
                    }
                    Rectangle {
                        width: parent.width / 3
                        height: parent.height
                        radius: 5
                        color: root.puzzleMode === 2 ? "#3b82f6" : "transparent"
                        Text {
                            anchors.centerIn: parent
                            text: "Ghosts"
                            color: root.puzzleMode === 2 ? "white" : "#999999"
                            font.pixelSize: 12
                            font.bold: root.puzzleMode === 2
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: root.puzzleMode = 2
                        }
                    }
                }
            }

            Rectangle {
                id: resetButton
                width: 90
                height: 34
                radius: 6
                color: "#2a2a2a"
                border.color: "#444444"
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    anchors.centerIn: parent
                    text: "Reset"
                    color: "#dddddd"
                    font.pixelSize: 13
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (root.puzzleMode === 0)
                            outsideInput.reset()
                        else if (root.puzzleMode === 1)
                            insidePanel.reset()
                        else
                            ghostHelper.reset()
                    }
                }
            }
        }

        // Outside puzzle: two-panel layout, side-by-side on wide windows,
        // stacked on narrow/mobile ones.
        Item {
            width: parent.width
            height: parent.height - y
            visible: root.puzzleMode === 0

            Flow {
                anchors.fill: parent
                spacing: 12

                InputPanel {
                    id: outsideInput
                    width: root.wideLayout ? (parent.width - 12) * 0.42 : parent.width
                    height: root.wideLayout ? parent.height : implicitHeight
                    stepCalculator: stepCalculator
                    encounterProgress: encounterProgress
                }

                SolutionPanel {
                    width: root.wideLayout ? (parent.width - 12) * 0.58 : parent.width
                    height: root.wideLayout ? parent.height : 460
                    stepCalculator: stepCalculator
                }
            }
        }

        // Inside puzzle
        InsideRoomPanel {
            id: insidePanel
            width: parent.width
            height: parent.height - y
            visible: root.puzzleMode === 1
            insideCalculator: insideStepCalculator
            encounterProgress: encounterProgress
        }

        // Ghost phase helper
        GhostPhaseHelperPanel {
            width: parent.width
            height: parent.height - y
            visible: root.puzzleMode === 2
            ghostHelper: ghostHelper
        }

        ShapeUsageTracker {
            width: parent.width
            progress: encounterProgress
            visible: root.puzzleMode !== 2 && encounterProgress.challengeModeEnabled
        }
    }
}
