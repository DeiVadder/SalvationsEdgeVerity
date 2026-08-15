import QtQuick
import QtQuick.Window
import QtQuick.Controls

import CalculateSteps 1.0

import "qml/panels"
import "qml/challenge"
import "qml/ghost"
import "qml/help"

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
                width: parent.width - modeToggle.width - helpButton.width - languageToggle.width - resetButton.width - 48
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
                            text: qsTr("Outside")
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
                            text: qsTr("Inside")
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
                            text: qsTr("Ghosts")
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
                id: helpButton
                width: 34
                height: 34
                radius: 6
                color: "#161616"
                border.color: "#333333"
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    anchors.centerIn: parent
                    text: "?"
                    color: "#dddddd"
                    font.pixelSize: 15
                    font.bold: true
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: helpDialog.open()
                }
            }

            Rectangle {
                id: languageToggle
                width: 68
                height: 34
                radius: 6
                color: "#161616"
                border.color: "#333333"
                anchors.verticalCenter: parent.verticalCenter

                Row {
                    anchors.centerIn: parent
                    spacing: 6

                    Rectangle {
                        width: 26
                        height: 20
                        radius: 3
                        color: "transparent"
                        border.color: translationManager.currentLanguage === "en" ? "#3b82f6" : "transparent"
                        border.width: 2
                        anchors.verticalCenter: parent.verticalCenter

                        Image {
                            anchors.centerIn: parent
                            width: 20
                            height: 13
                            fillMode: Image.PreserveAspectFit
                            source: "qrc:/images/svg/flags/flag_en.svg"
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: translationManager.setLanguage("en")
                        }
                    }

                    Rectangle {
                        width: 26
                        height: 20
                        radius: 3
                        color: "transparent"
                        border.color: translationManager.currentLanguage === "de" ? "#3b82f6" : "transparent"
                        border.width: 2
                        anchors.verticalCenter: parent.verticalCenter

                        Image {
                            anchors.centerIn: parent
                            width: 20
                            height: 13
                            fillMode: Image.PreserveAspectFit
                            source: "qrc:/images/svg/flags/flag_de.svg"
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: translationManager.setLanguage("de")
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
                    text: qsTr("Reset")
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

        // Outside puzzle: two-panel layout, side-by-side on wide windows.
        // On narrow ones, side-by-side would squeeze both panels into
        // unusable slivers, and simply stacking them risks pushing the
        // second one below the visible window with no way to scroll back
        // up/down to it - so instead exactly one full-size panel is shown
        // at a time, switched via a tab bar. Single InputPanel/SolutionPanel
        // instances throughout (repositioned/resized, never duplicated) -
        // two separate instances would each hold their own independent
        // input state and fight over the shared stepCalculator.
        Item {
            id: outsideContainer
            width: parent.width
            height: parent.height - y
            visible: root.puzzleMode === 0

            property int tab: 0 // 0 = selection, 1 = solution - only used when narrow

            Rectangle {
                id: outsideTabBar
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
                        color: outsideContainer.tab === 0 ? "#3b82f6" : "transparent"
                        Text {
                            anchors.centerIn: parent
                            text: qsTr("Selection")
                            color: outsideContainer.tab === 0 ? "white" : "#999999"
                            font.pixelSize: 12
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: outsideContainer.tab = 0
                        }
                    }
                    Rectangle {
                        width: parent.width / 2
                        height: parent.height
                        radius: 5
                        color: outsideContainer.tab === 1 ? "#3b82f6" : "transparent"
                        Text {
                            anchors.centerIn: parent
                            text: qsTr("Solution")
                            color: outsideContainer.tab === 1 ? "white" : "#999999"
                            font.pixelSize: 12
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: outsideContainer.tab = 1
                        }
                    }
                }
            }

            InputPanel {
                id: outsideInput
                stepCalculator: stepCalculator
                encounterProgress: encounterProgress
                x: 0
                y: root.wideLayout ? 0 : outsideTabBar.height + 8
                width: root.wideLayout ? (parent.width - 12) * 0.42 : parent.width
                height: parent.height - y
                visible: root.wideLayout || outsideContainer.tab === 0
            }

            SolutionPanel {
                stepCalculator: stepCalculator
                x: root.wideLayout ? outsideInput.width + 12 : 0
                y: root.wideLayout ? 0 : outsideTabBar.height + 8
                width: root.wideLayout ? (parent.width - 12) * 0.58 : parent.width
                height: parent.height - y
                visible: root.wideLayout || outsideContainer.tab === 1
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

    HelpDialog {
        id: helpDialog
    }
}
