import QtQuick 2.15
import QtQuick.Window 2.15

import QtQuick.Controls 2.15

import CalculateSteps 1.0
import SymbolEnums 1.0

import "../shapes"
import "../steps"

Item{
    id:steps
    height:parent.height
    width: Math.min(parent.width, parent.height)

    property int itemWidth: 100
    property int itemHeight: Math.min(height / 4, height / (numberOfSteps + 1))

    property int numberOfSteps: 3

    property var toDunk1: []
    property var toDunk2: []
    property var toDunk3: []
    property var result: []

    property int finalSymbol1:0
    property int finalSymbol2:0
    property int finalSymbol3:0

    Row{
        anchors.fill: parent

        Repeater{
            model: 3
            delegate: oneColumn
        }
    }
    Component {
        id:oneColumn
        Column{
            id:column
            property int toDunkIndex: index
            Repeater {
                model: numberOfSteps

                delegate: Item {
                    id:stepItem
                    height: steps.itemHeight
                    width: steps.itemWidth
                    property int repeaterIndex: index
                    StepX {
                        id: stepX
                        sX: index + 1
                        height: parent.height * 0.9
                        width: parent.width

                        property int stepIndex: stepItem.repeaterIndex


                        symbolToDunk: column.toDunkIndex == 0 ? steps.toDunk1[stepIndex] : column.toDunkIndex == 1 ? steps.toDunk2[stepIndex] : steps.toDunk3[stepIndex]
                        onSymbolToDunkChanged: console.log("Statue:", column.toDunkIndex, steps.toDunk1, steps.toDunk2, steps.toDunk3)
                    }

                    ToolSeparator {
                        anchors.top: stepX.bottom
                        width: steps.itemWidth
                        orientation: Qt.Horizontal
                    }
                }
            }

            Rectangle {
                id:finsihed
                height: steps.itemHeight * 0.7
                width: steps.itemWidth

                property int finalSymbol: index == 0 ? finalSymbol1 : index == 1? finalSymbol2 : finalSymbol3
                visible: numberOfSteps > 0

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
}
