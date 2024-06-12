import QtQuick 2.15
import QtQuick.Window 2.15

Item {
    id:selection3dShape
    property int selection: -1

    Rectangle {
        anchors.fill: parent
        // anchors.margins:  2
        anchors.margins: selection3dShape.height / 8
        border.color: "black"
        border.width: shape3dSelection.source != ""? 0 : 1

        Text {
            text:  qsTr("Click to Select")
            anchors.centerIn: parent
        }

        Image {
            id:shape3dSelection
            anchors.fill: parent

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
