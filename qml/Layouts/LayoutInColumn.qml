import QtQuick 2.15
import QtQuick.Window 2.15

import QtQuick.Controls 2.15

import CalculateSteps 1.0
import SymbolEnums 1.0


Flickable {
    id:root

    signal openPopup()
    function reset() {
        statueStateMachine1.reset()
        statueStateMachine2.reset()
        statueStateMachine3.reset()
    }

    property var stc: undefined
    property int itemWidth: 100//(appFrame.smallestSide - 20) / 3

    contentHeight: statueStateMachine1.height

    function checkBeforCalculation() {
        console.log("checkBeforCalculation")

        if(statueStateMachine1.insideSymbol <= 0 ||
                statueStateMachine2.insideSymbol <= 0 ||
                statueStateMachine3.insideSymbol <= 0 ||
                statueStateMachine1.outSideSymbol <= 0 ||
                statueStateMachine2.outSideSymbol <= 0 ||
                statueStateMachine3.outSideSymbol <= 0)
            return
        if(stc.checkIsValid(statueStateMachine1.insideSymbol, statueStateMachine2.insideSymbol, statueStateMachine3.insideSymbol,
                                       statueStateMachine1.outSideSymbol, statueStateMachine2.outSideSymbol, statueStateMachine3.outSideSymbol)) {
            stc.calculateSteps(statueStateMachine1.insideSymbol, statueStateMachine2.insideSymbol, statueStateMachine3.insideSymbol,
                                          statueStateMachine1.outSideSymbol, statueStateMachine2.outSideSymbol, statueStateMachine3.outSideSymbol)

            var toDunkStatue1 = [];
            var toDunkStatue2 = [];
            var toDunkStatue3 = [];
            for(let i = 0; i < stc.numberOfSteps; i++){
                toDunkStatue1.push(stc.getInstructionForStep(i, 0))
                toDunkStatue2.push(stc.getInstructionForStep(i, 1))
                toDunkStatue3.push(stc.getInstructionForStep(i, 2))
            }

            statueStateMachine1.toDunk = toDunkStatue1
            statueStateMachine2.toDunk = toDunkStatue2
            statueStateMachine3.toDunk = toDunkStatue3
        } else {
            root.openPopup()
        }
    }

    Row {
        id:row
        spacing: 10
        property int itemWidth: root.itemWidth
        VisualStateMachineForCol {
            id: statueStateMachine1
            width: row.itemWidth

            statueIndex: qsTr("left")
            numberOfSteps: stc.numberOfSteps

            onInsideSymbolChanged: checkBeforCalculation()
            onOutSideSymbolChanged: checkBeforCalculation()
        }

        VisualStateMachineForCol {
            id: statueStateMachine2
            width: row.itemWidth

            statueIndex: qsTr("middle")
            numberOfSteps: stc.numberOfSteps

            onInsideSymbolChanged: checkBeforCalculation()
            onOutSideSymbolChanged: checkBeforCalculation()
        }

        VisualStateMachineForCol {
            id: statueStateMachine3
            width: row.itemWidth

            statueIndex: qsTr("right")
            numberOfSteps: stc.numberOfSteps


            onInsideSymbolChanged: checkBeforCalculation()
            onOutSideSymbolChanged: checkBeforCalculation()
        }
    }
}
