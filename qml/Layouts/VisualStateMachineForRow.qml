import QtQuick 2.15
import QtQuick.Controls 2.15

import "../shapes"
import "../steps"

Item {
    id:statemachineRow

    //Set from outside
    property string statueIndex: ""
    property int numberOfSteps: 0
    property var toDunk: []
    property var result: []

    //Set from inside
    property int insideSymbol: 0
    property int outSideSymbol: 0


    //inside varbiables/const
    property int itemWidth: height
    property alias stateTextWidth: header.width

    //legend/header alignment
    readonly property var stateText: header
    readonly property var innerSelection: selection2d
    readonly property var finishedSymbol: finsihed

    width: finsihed.x + finsihed.width

    function reset() {
        selection2d.reset()
        selection3dShape.selection = -1
    }

    Row {

        //Statue
        Text {
            id:header
            text: qsTr("Statue %1").arg(statueIndex)

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            height: statemachineRow.height
            width: contentWidth * 1.2

            font.pixelSize: 20
        }

        ToolSeparator {
            height: statemachineRow.height
        }

        //inside selection
        Selection2d {
            id:selection2d
            height: statemachineRow.height
            width: statemachineRow.itemWidth * 0.7
            onSelectionChanged:{
                finsihed.finalSymbol = selection
                insideSymbol = selection

            }
        }

        //outside selection

        Selection3dShape {
            Text {
                id: outsideSymbolText
                anchors.top: parent.top
                text: qsTr("Outside symbol:")
                topPadding: 5


                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 15
                width: parent.width
                wrapMode: Text.WordWrap

            }
            id: selection3dShape
            width: statemachineRow.itemWidth * 0.7
            height: statemachineRow.height
            onSelectionChanged: {
                if(selection < 0)
                    outSideSymbol = selection
                else
                    outSideSymbol = selection +4
            }
        }
        ToolSeparator {
            height: statemachineRow.height
        }

        //Steps for the outside team
        Repeater {
            model: numberOfSteps

            delegate: Item {
                width: statemachineRow.itemWidth
                height: statemachineRow.height
                StepX {
                    id: stepX
                    sX: index + 1
                    width: parent.width * 0.9
                    height: statemachineRow.height

                    symbolToDunk: toDunk[index]
                    // resultingSymbol: result[index]
                }

                ToolSeparator {
                    anchors.left: stepX.right
                    height: statemachineRow.height
                }
            }
        }

        Rectangle {
            id:finsihed
            height: statemachineRow.height
            width: statemachineRow.itemWidth * 0.7

            property int finalSymbol: 0
            visible: numberOfSteps > 0

            Text {
                id: idText
                text: qsTr("Finished symbol")
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.top: parent.top
                height: contentHeight * 2
                width: parent.width

                font.pixelSize: 15
            }

            Rectangle {
                // border.width: 1
                // border.color: "black"
                anchors{
                    top: idText.bottom
                    left:parent.left
                    bottom: parent.bottom
                    right: parent.right
                    margins: 1
                }

                Image {
                    id:targetSymbol
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit

                    function targetShape (index) {
                        if(index == 1)
                            return "qrc:/images/Zylinder.png"
                        if(index == 2)
                            return "qrc:/images/Kegel.png"
                        if(index == 3)
                            return "qrc:/images/Prisma.png"
                        return ""
                    }

                    source:targetShape(finsihed.finalSymbol)

                }
            }
        }
    }
}
