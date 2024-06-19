import QtQuick 2.15
import QtQuick.Window 2.15

import QtQuick.Controls 2.15

import CalculateSteps 1.0
import SymbolEnums 1.0

import "../shapes"
import "../steps"

Item {
    id:root

    signal openPopup()

    property int itemSize:(Math.min(width, height) - 20) / 3
    property var stc: undefined

    function reset() {
        input.reset()

    }

    function checkBeforCalculation() {
        console.log("checkBeforCalculation")
        console.log(input.insideSymbol1 ,
                    input.insideSymbol2 ,
                    input.insideSymbol3 ,
                    input.outSideSymbol1,
                    input.outSideSymbol2 ,
                    input.outSideSymbol3 )

        if(input.insideSymbol1 <= 0 ||
                input.insideSymbol2 <= 0 ||
                input.insideSymbol3 <= 0 ||
                input.outSideSymbol1 <= 0 ||
                input.outSideSymbol2 <= 0 ||
                input.outSideSymbol3 <= 0)
            return
        if(stc.checkIsValid(input.insideSymbol1, input.insideSymbol2, input.insideSymbol3,
                                       input.outSideSymbol1, input.outSideSymbol2, input.outSideSymbol3)) {
            stc.calculateSteps(input.insideSymbol1, input.insideSymbol2, input.insideSymbol3,
                                          input.outSideSymbol1, input.outSideSymbol2, input.outSideSymbol3)

            var toDunkStatue1 = [];
            var toDunkStatue2 = [];
            var toDunkStatue3 = [];
            for(let i = 0; i < stc.numberOfSteps; i++){
                toDunkStatue1.push(stc.getInstructionForStep(i, 0))
                toDunkStatue2.push(stc.getInstructionForStep(i, 1))
                toDunkStatue3.push(stc.getInstructionForStep(i, 2))
            }

            vSteps.toDunk1 = toDunkStatue1
            vSteps.toDunk2 = toDunkStatue2
            vSteps.toDunk3 = toDunkStatue3

            console.log("toDunk",toDunkStatue1, toDunkStatue2, toDunkStatue3)
        } else {
            root.openPopup()
            // popup.open()
        }
    }

    Row{
        anchors.fill: parent
        InputSymbols {
            id: input
            height: parent.height
            width: Math.min(parent.width, parent.height)
            itemWidth: itemSize

            onInsideSymbol1Changed: checkBeforCalculation()
            onInsideSymbol2Changed: checkBeforCalculation()
            onInsideSymbol3Changed: checkBeforCalculation()
            onOutSideSymbol1Changed: checkBeforCalculation()
            onOutSideSymbol2Changed: checkBeforCalculation()
            onOutSideSymbol3Changed: checkBeforCalculation()
        }

        ToolSeparator {
            height: root.height
            orientation: Qt.Vertical
        }

        VisualSteps {
            id: vSteps
            itemWidth: itemSize
            height: parent.height
            width: Math.min(parent.width, parent.height)

            numberOfSteps:stc.numberOfSteps
            finalSymbol1: input.insideSymbol1
            finalSymbol2: input.insideSymbol2
            finalSymbol3: input.insideSymbol3
        }
    }

}
