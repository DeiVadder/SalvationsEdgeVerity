import QtQuick 2.15

Rectangle {
    // border.color: "red"
    // border.width: 1
    property int contentHeight: selected.y + selected.contentHeight

    function reset() {
        selection = 0
    }

    property int selection: 0
    onSelectionChanged: console.log("selection changed", selection)
    clip :true

    ShapePopup2d {
        id: doSelection
        anchors.fill: parent

        visible: selection == 0

        onSelected: selection = index

    }

    Rectangle {
        id: selected
        anchors.fill: parent
        visible: selection != 0

        property int contentHeight: targetSymbol.y + targetSymbol.height

        Item{
            id:frameInside
            width: parent.width / 2
            x: verbose ? 1 : parent.width / 4

            Text{
                id:iSymbolText
                width: selectedSymbol.width
                height: contentHeight * 2
                anchors.horizontalCenter: selectedSymbol.horizontalCenter
                anchors.top: parent.top
                text: qsTr("Inside Symbol:")


                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }

            Image {
                id:selectedSymbol
                width: frameInside.width -1
                anchors{
                    left:parent.left
                    top: iSymbolText.bottom
                }

                fillMode: Image.PreserveAspectFit

                source: {
                    switch(selection) {
                    case 0: return ""
                    case 1: return "qrc:/images/Dreieck.png"
                    case 2: return "qrc:/images/Viereck.png"
                    case 3: return "qrc:/images/Kreis.png"
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: selection = 0
                }
            }
        }


        Item{
            id:frameOutside
            visible: verbose
            width: parent.width / 2
            anchors{
                left: parent.horizontalCenter
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            Text {
                id:targetSymbolText
                width: targetSymbol.width
                height: contentHeight * 2
                anchors.horizontalCenter: targetSymbol.horizontalCenter
                anchors.top: parent.top
                text: qsTr("Outside target:")


                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }

            Image {
                id:targetSymbol
                width: frameOutside.width -1
                anchors{
                    right:parent.right
                    top:targetSymbolText.bottom
                }
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

                source:targetShape(selection)
            }
        }

    }
}
