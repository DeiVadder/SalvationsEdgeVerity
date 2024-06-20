import QtQuick 2.15
import QtQuick.Window 2.15

import QtQuick.Controls 2.15

import CalculateSteps 1.0
import SymbolEnums 1.0

import "../shapes"
import "../steps"

Item{
    id:input

    property int itemWidth: 100
    property int itemHeight: Math.max(itemWidth, height / 2)

    property int insideSymbol1: 0
    property int outSideSymbol1: 0

    property int insideSymbol2: 0
    property int outSideSymbol2: 0

    property int insideSymbol3: 0
    property int outSideSymbol3: 0

    signal reset()

    Row{
        anchors.fill: parent

        Repeater{
            model: 3
            delegate: oneColumn
        }

    }

    Component {
        id:oneColumn
        Column {
            id:col1
            width: itemWidth
            height: input.hight

            property int repeaterIndex: index
            Text {
                id:header
                text: qsTr("Statue %1").arg(index == 0 ? "left" : index == 1 ? "middle" : "right")

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                width: col1.width
                height: contentHeight * 2
                font.pixelSize: fontTitle
            }

            ToolSeparator {
                width: col1.width
                orientation: Qt.Horizontal
            }

            Selection2d {
                id:selection2d
                width: col1.width
                height: input.itemHeight * 0.7

                Connections{
                    target: input
                    function onReset() {
                        selection2d.reset()
                    }
                }

                // iconHeight: stepX.iconHeight
                onSelectionChanged:{
                    // finsihed.finalSymbol = selection
                    switch(col1.repeaterIndex ){
                    case 0: insideSymbol1 = selection; break;
                    case 1: insideSymbol2 = selection; break;
                    case 2: insideSymbol3 = selection; break;
                    }


                }
            }

            ToolSeparator {
                width: col1.width
                orientation: Qt.Horizontal
            }

            Selection3dShape {
                id: selection3dShape
                height: input.itemHeight * 0.7
                width: col1.width
                onSelectionChanged: {

                    if(col1.repeaterIndex == 0) {
                        if(selection < 0)
                            outSideSymbol1 = selection
                        else
                            outSideSymbol1 = selection +4
                    } if (col1.repeaterIndex  == 1) {
                        if(selection < 0)
                            outSideSymbol2 = selection
                        else
                            outSideSymbol2 = selection +4
                    } if (col1.repeaterIndex  == 2) {
                        if(selection < 0)
                            outSideSymbol3 = selection
                        else
                            outSideSymbol3 = selection +4
                    }
                }

                Connections{
                    target: input
                    function onReset() {
                        selection3dShape.selection = -1
                    }
                }
            }

            ToolSeparator {
                width: col1.width
                orientation: Qt.Horizontal
            }
        }
    }
}
