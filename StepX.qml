import QtQuick 2.15
import QtQuick.Window 2.15

import QtQuick.Controls 2.15

Rectangle {
    id: stepX
    property int sX: 1
    property int symbolToDunk: 1
    property int resultingSymbol: 5

    readonly property int iconHeight: stepsToDo.height * 0.75

    Text {
        id: idText
        text: qsTr("Step %1").arg(stepX.sX)
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.top: parent.top
        height: contentHeight * 1.2
        width: parent.width

        font.pixelSize: 20
    }

    Rectangle {
        border.width: 1
        border.color: "black"
        visible: stepX.symbolToDunk == 0
        anchors{
            top: idText.bottom
            left:parent.left
            bottom: parent.bottom
            right: parent.right
            margins: parent.height / 8
            leftMargin: parent.width / 4
            rightMargin: parent.width / 4
        }

        Text {
            text: qsTr("Nothing to do here")
            anchors.fill:parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            wrapMode: Text.WordWrap
        }
    }

    Rectangle {
        id: stepsToDo
        anchors{
            top: idText.bottom
            left:parent.left
            bottom: parent.bottom
            right: parent.right
            margins: 1
        }
        visible: stepX.symbolToDunk != 0

        Item {
            id: frameToDunk
            x: verbose ? 0 : parent.width / 4
            anchors{
                // left: verbose ? parent.left : undefined
                // right: verbose ? parent.horizontalCenter : undefined
                top: parent.top
                bottom: parent.bottom
                // horizontalCenter: verbose ? undefined : parent.horizontalCenter
            }
            width: parent.width / 2

            Text{
                width: selectedSymbol.width
                height: contentHeight * 1.2
                anchors.horizontalCenter: selectedSymbol.horizontalCenter
                anchors.bottom: selectedSymbol.top
                text: qsTr("Dunk symbol at statue:")
                wrapMode: Text.WordWrap

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
            Image {
                id:selectedSymbol
                height: parent.height * 0.75
                width: parent.width - 2
                anchors{
                    left:parent.left
                    bottom: parent.bottom
                }

                fillMode: Image.PreserveAspectFit

                source: {
                    switch(stepX.symbolToDunk) {
                    case 0: return ""
                    case 1: return "qrc:/images/Dreieck.png"
                    case 2: return "qrc:/images/Viereck.png"
                    case 3: return "qrc:/images/Kreis.png"
                    }
                }
            }
        }

        Item{
            id:frameTarget
            visible: verbose

            anchors{
                left: parent.horizontalCenter
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            Text {
                width: targetSymbol.width
                height: contentHeight * 1.2
                anchors.horizontalCenter: targetSymbol.horizontalCenter
                anchors.bottom: targetSymbol.top
                text: qsTr("Symbol when Step finsihed:")
                wrapMode: Text.WordWrap

                visible: verbose


                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }

            Image {
                id:targetSymbol
                height: parent.height * 0.75
                width: parent.width - 2
                anchors{
                    right:parent.right
                    bottom: parent.bottom

                }

                visible: verbose

                fillMode: Image.PreserveAspectFit

                source: {
                    switch(stepX.resultingSymbol) {
                    default: return ""
                    case 4: return "qrc:/images/Kegel.png"
                    case 5: return "qrc:/images/Wuerfel.png"
                    case 6: return "qrc:/images/Zylinder.png"
                    case 7: return "qrc:/images/Pyramide.png"
                    case 8: return "qrc:/images/Prisma.png"
                    case 9: return "qrc:/images/Kugel.png"
                    }
                }
            }
        }


    }
}
