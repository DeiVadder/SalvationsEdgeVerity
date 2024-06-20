import QtQuick 2.15
import QtQuick.Window 2.15

Item {
    id:selection3dShape
    property int selection: -1
    property int headerOffset: 0

    Rectangle {
        id:selected
        anchors.fill: parent
        // anchors.margins:  2
        // anchors.margins: selection3dShape.height / 8
        // border.color: "black"
        // border.width: shape3dSelection.source != ""? 0 : 1

        Item{
            id:frameOutside
            width: parent.width / 2
            x: parent.width / 4
            height: parent.height
            visible: !shapePopup.visible
            Text{
                id:iSymbolText
                width: shape3dSelection.width
                // height: contentHeight * 2
                height: contentHeight * 1.1
                anchors.horizontalCenter: shape3dSelection.horizontalCenter
                anchors.top: parent.top
                text: qsTr("Outside Symbol:")
                // wrapMode: Text.WordWrap
                font.pixelSize: 15


                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
            Image {
                id:shape3dSelection
                anchors{
                    top: iSymbolText.bottom
                    left:parent.left
                    right: parent.right
                    bottom: parent.bottom
                }

                fillMode: Image.PreserveAspectFit

                source: {
                    switch(selection3dShape.selection + 4) {
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


        MouseArea {
            anchors.fill: parent
            onClicked: selection3dShape.selection = -1
        }

        ShapePopup3d {
            id:shapePopup
            anchors.fill: parent

            visible: selection == -1

            onSelection3d:{
                selection3dShape.selection = index

            }
        }
    }
}
