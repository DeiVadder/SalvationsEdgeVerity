import QtQuick 2.15
import QtQuick.Window 2.15

import QtQuick.Controls 2.15

Item {
    id: statueStateMachine

    height: finsihed.y + finsihed.height

    property int headerHeight: header.height

    property int statueIndex: 1
    property int itemHeight: width

    function reset() {
        selection2d.reset()
        selection3dShape.selection = -1
        // outSideSymbol = -1
    }

    property int insideSymbol: 0
    property int outSideSymbol: 0

    property var toDunk: []
    property var result: []
    // property var stepPairs: [[1,5], [2,6], [0,0]]

    Column {
        Text {
            id:header
            text: qsTr("Statue %1").arg(statueIndex)

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            width: statueStateMachine.width
            height: contentHeight * 2
            font.pixelSize: fontTitle
        }

        ToolSeparator {
            width: statueStateMachine.width
            orientation: Qt.Horizontal
        }

        Selection2d {
            id:selection2d
            width: statueStateMachine.width
            height: statueStateMachine.itemHeight * 0.7
            // iconHeight: stepX.iconHeight
            onSelectionChanged:{
                console.log("selection", selection)
                finsihed.finalSymbol = selection
                insideSymbol = selection

            }
        }

        Text {
            id: outsideSymbolText
            text: selection3dShape.selection == -1 ? qsTr("Select outside symbol") :  qsTr("Outside symbol:")

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            width: statueStateMachine.width
            height: contentHeight * 2
            font.pixelSize: fontNormal
        }

        Selection3dShape {
            id: selection3dShape
            height: statueStateMachine.itemHeight * 0.7
            width: statueStateMachine.width
            onSelectionChanged: {
                console.log("Outside Symbol", selection)
                if(selection < 0)
                    outSideSymbol = selection
                else
                    outSideSymbol = selection +4
            }
        }

        ToolSeparator {
            width: statueStateMachine.width
            orientation: Qt.Horizontal
        }

        Repeater {
            model: stepCalculator.numberOfSteps

            delegate: Item {
                height: statueStateMachine.itemHeight
                width: statueStateMachine.width
                StepX {
                    id: stepX
                    sX: index + 1
                    height: parent.height * 0.9
                    width: statueStateMachine.width

                    symbolToDunk: toDunk[index]
                    // resultingSymbol: result[index]
                }

                ToolSeparator {
                    anchors.top: stepX.bottom
                    width: statueStateMachine.width
                    orientation: Qt.Horizontal
                }
            }
        }

        Rectangle {
            id:finsihed
            height: statueStateMachine.itemHeight * 0.7
            width: statueStateMachine.width

            property int finalSymbol: 0
            visible: stepCalculator.numberOfSteps > 0

            Text {
                id: idText
                text: qsTr("Finished symbol")
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.top: parent.top
                height: contentHeight * 2
                width: parent.width

                font.pixelSize: fontNormal
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
