import QtQuick 2.15
import QtQuick.Window 2.15

import QtQuick.Controls 2.15

import CalculateSteps 1.0
import SymbolEnums 1.0

import "qml/Layouts"

Window {
    id:root
    width: 750
    height: 600
    visible: true
    title: qsTr("Salvations Edge Verity Encounter")

    property bool verbose: false
    property int layoutOrientation: 0
    property double scale: 1

    property int fontNormal:15 * scale
    property int fontTitle: 20 * scale

    function checkBeforCalculation() {

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

    signal reset()


    Item {
        id: appFrame
        height: root.height * root.scale
        width: root.width * root.scale/*Math.min(root.width, root.height)*/
        y: smallestSide - height

        property int smallestSide: Math.min(height, width)

        CalculateSteps{
            id:stepCalculator
        }

        Loader{
            id: layoutLoader
            anchors.fill: parent
            anchors.margins: 5

            sourceComponent: {
                if(layoutOrientation ==0)
                    return layoutInRowColComponent
                if(layoutOrientation == 1)
                    return layoutInColComponent
                return layoutInRowComponent;
            }
        }

        Component{
            id:layoutInRowComponent
            LayoutInRow {
                id:layoutInRow
                anchors.fill: parent

                itemHeight: ( appFrame.height - 20 ) / 3
                stc:stepCalculator

                onOpenPopup: popup.open()

                Connections{
                    target: root
                    function onReset() {
                        layoutInRow.reset()
                    }
                }
            }
        }

        Component{
            id:layoutInColComponent
            LayoutInColumn {
                id:layoutInCol
                anchors.fill: parent
                itemWidth: ( appFrame.height - 20 ) / 3
                stc:stepCalculator

                onOpenPopup: popup.open()
                Connections{
                    target: root
                    function onReset() {
                        layoutInCol.reset()
                    }
                }
            }
        }

        Component {
            id: layoutInRowColComponent
            LayoutInRowCol {
                id:layoutInRowCol
                anchors.fill: parent
                stc:stepCalculator

                onOpenPopup: popup.open()

                Connections{
                    target: root
                    function onReset() {
                        layoutInRowCol.reset()
                    }
                }
            }
        }


    }

    IconButton {
        id:btnReset
        anchors{
            top: parent.top
            right:parent.right
        }
        width:40
        height: width
        imgMargin: 0/*height * 0.1*/

        colorNormal: "transparent"
        colorPressed: "grey"

        source: "qrc:/images/reset.svg"
        z:1
        onClicked: {
            stepCalculator.reset()
            root.reset()
        }
    }

    IconButton {
        id:orientation
        anchors{
            top: btnReset.bottom
            // left: parent.left
            right:parent.right
        }
        width:/*statueStateMachine1.headerHeight*//* * 0.6*/40
        height: width
        imgMargin:0

        colorNormal: "transparent"
        colorPressed: "grey"

         z:1
        source:{
            if(layoutOrientation ==0)
                return "qrc:/images/orientationRowCol.png"
            if(layoutOrientation == 1)
                return "qrc:/images/orientationCol.png"
            return "qrc:/images/orientationRow.png"
        }
        onClicked: {
            stepCalculator.reset()
            root.reset()
            if(layoutOrientation >= 2)
                layoutOrientation =0
            else
                layoutOrientation++
        }
    }

    Button{
        id:mBtn
        text: "-"
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: 40
        height: width
        z: 1
        visible: root.scale > 0.1
        onClicked:{
            root.scale = root.scale - 0.05
        }
    }
    Button{
        id:pBtn
        text: "+"
        anchors.bottom: mBtn.top
        anchors.right: parent.right
        width: 40
        height: width
        z: 1
        visible: root.scale < 1
        onClicked:{
            root.scale = root.scale + 0.05
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
                font.pixelSize: 20
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
