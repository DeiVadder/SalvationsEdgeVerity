import QtQuick 2.15
import QtQuick.Window 2.15

import QtQuick.Controls 2.15

import CalculateSteps 1.0
import SymbolEnums 1.0

Window {
    id:root
    width: 750
    height: 600
    visible: true
    title: qsTr("Salvations Edge Verity Encounter")

    property bool verbose: false

    function checkBeforCalculation() {
        console.log("checkBeforCalculation")

        if(statueStateMachine1.insideSymbol <= 0 ||
                statueStateMachine2.insideSymbol <= 0 ||
                statueStateMachine3.insideSymbol <= 0 ||
                statueStateMachine1.outSideSymbol <= 0 ||
                statueStateMachine2.outSideSymbol <= 0 ||
                statueStateMachine3.outSideSymbol <= 0)
            return
        if(stepCalculator.checkIsValid(statueStateMachine1.insideSymbol, statueStateMachine2.insideSymbol, statueStateMachine3.insideSymbol,
                                       statueStateMachine1.outSideSymbol, statueStateMachine2.outSideSymbol, statueStateMachine3.outSideSymbol)) {
            stepCalculator.calculateSteps(statueStateMachine1.insideSymbol, statueStateMachine2.insideSymbol, statueStateMachine3.insideSymbol,
                                          statueStateMachine1.outSideSymbol, statueStateMachine2.outSideSymbol, statueStateMachine3.outSideSymbol)

            var toDunkStatue1 = [];
            var toDunkStatue2 = [];
            var toDunkStatue3 = [];
            for(let i = 0; i < stepCalculator.numberOfSteps; i++){
                toDunkStatue1.push(stepCalculator.getInstructionForStep(i, 0))
                toDunkStatue2.push(stepCalculator.getInstructionForStep(i, 1))
                toDunkStatue3.push(stepCalculator.getInstructionForStep(i, 2))
            }

            statueStateMachine1.toDunk = toDunkStatue1
            statueStateMachine2.toDunk = toDunkStatue2
            statueStateMachine3.toDunk = toDunkStatue3
        } else {
            popup.open()
        }
    }

    Item {
        id: appFrame
        height: root.height
        width: Math.min(root.width, root.height)

        IconButton {
            id:btnReset
            anchors{
                top: parent.top
                left: parent.left
            }
            width:statueStateMachine1.headerHeight * 0.6
            height: width
            imgMargin: height * 0.1

            // border.color: "black"
            // border.width: 1
            colorNormal: "white"
            colorPressed: "grey"

            source: "qrc:/images/reset.svg"
            z:1
            onClicked: {
                stepCalculator.reset()
                statueStateMachine1.reset()
                statueStateMachine2.reset()
                statueStateMachine3.reset()
            }
        }

        CalculateSteps{
            id:stepCalculator
        }

        Flickable {
            anchors.fill: parent
            contentHeight: statueStateMachine1.height

            Row {
                id:row
                spacing: 10
                property int itemWidth: (appFrame.width - 20) / 3
                StatueStateMachine {
                    id: statueStateMachine1
                    width: row.itemWidth

                    statueIndex: 1

                    onInsideSymbolChanged: checkBeforCalculation()
                    onOutSideSymbolChanged: checkBeforCalculation()
                }

                StatueStateMachine {
                    id: statueStateMachine2
                    width: row.itemWidth

                    statueIndex: 2

                    onInsideSymbolChanged: checkBeforCalculation()
                    onOutSideSymbolChanged: checkBeforCalculation()
                }

                StatueStateMachine {
                    id: statueStateMachine3
                    width: row.itemWidth

                    statueIndex: 3


                    onInsideSymbolChanged: checkBeforCalculation()
                    onOutSideSymbolChanged: checkBeforCalculation()
                }
            }
        }


    }
    Popup {
        id: popup
        x: (root.width - width) /2
        y: (root.height - height) / 4
        width: root.width / 3
        height: root.width / 5
        modal: true
        visible: false

        Rectangle {
            width: parent.width
            height: parent.height
            color: "lightgrey"

            Text {
                anchors.centerIn: parent
                text: "Invalid input"
                font.pixelSize: 16
            }

            Button {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: 10
                }
                text: "OK"
                onClicked: popup.close()
            }
        }
    }
}
