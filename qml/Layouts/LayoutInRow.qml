import QtQuick 2.15
import QtQuick.Window 2.15

import QtQuick.Controls 2.15

import CalculateSteps 1.0
import SymbolEnums 1.0

Flickable {
    id:root

    signal openPopup()
    contentWidth: statueStateMachine1.width
    contentHeight: itemHeight  * 3

    property int itemHeight: 100//( appFrame.height - 20 ) / 3
    property var stc: undefined

    function reset() {
        statueStateMachine1.reset()
        statueStateMachine2.reset()
        statueStateMachine3.reset()
    }


    function checkBeforCalculation() {
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
            // popup.open()
        }
    }

    Column {
        id: col
        spacing: 10
        property int itemHeight:root.itemHeight
        VisualStateMachineForRow {
            id: statueStateMachine3
            height: col.itemHeight
            stateTextWidth: parent.minWidthHeader

            statueIndex: qsTr("right")
            numberOfSteps: stc.numberOfSteps


            onInsideSymbolChanged: checkBeforCalculation()
            onOutSideSymbolChanged: checkBeforCalculation()
        }

        property int minWidthHeader: Math.max(statueStateMachine1.stateText.contentWidth, Math.max(statueStateMachine2.stateText.contentWidth, statueStateMachine3.stateText.contentWidth))

        VisualStateMachineForRow {
            id: statueStateMachine2
            height: col.itemHeight
            stateTextWidth: parent.minWidthHeader

            statueIndex: qsTr("middle")
            numberOfSteps: stc.numberOfSteps


            onInsideSymbolChanged: checkBeforCalculation()
            onOutSideSymbolChanged: checkBeforCalculation()
        }

        VisualStateMachineForRow {
            id: statueStateMachine1
            stateTextWidth: parent.minWidthHeader
            height: col.itemHeight
            statueIndex: qsTr("left")
            numberOfSteps: stc.numberOfSteps

            onInsideSymbolChanged: checkBeforCalculation()
            onOutSideSymbolChanged: checkBeforCalculation()
        }
    }
}
