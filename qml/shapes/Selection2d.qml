import QtQuick 2.15

Rectangle {
    // border.color: "red"
    // border.width: 1

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

        Item{
            id:frameInside
            width: parent.width / 2
            x: parent.width / 4
            height: parent.height

            Text{
                id:iSymbolText
                width: selectedSymbol.width
                // height: contentHeight * 2
                height: contentHeight * 1.1
                anchors.horizontalCenter: selectedSymbol.horizontalCenter
                anchors.top: parent.top
                text: qsTr("Inside Symbol:")
                // wrapMode: Text.WordWrap
                font.pixelSize: fontNormal


                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }

            Image {
                id:selectedSymbol
                width: frameInside.width -1
                anchors{
                    top: iSymbolText.bottom
                    left:parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                Component.onCompleted: console.log("öö",width, height)

                fillMode: Image.PreserveAspectFit

                source: {
                    switch(selection) {
                    case 0: return ""
                    case 1: return "qrc:/images/Dreieck.png"
                    case 2: return "qrc:/images/Viereck.png"
                    case 3: return "qrc:/images/Kreis.png"
                    }
                }

            }
            MouseArea {
                anchors.fill: parent
                onClicked: selection = 0
            }
        }
    }
}
