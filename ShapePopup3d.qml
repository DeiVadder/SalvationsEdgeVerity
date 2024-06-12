import QtQuick 2.15

Rectangle {
    id:root3dPopup
    signal selection3d(var index)


    GridView {
        id:gView
        anchors.fill: parent
        model: 6

        cellWidth: width / 2
        cellHeight: height / 3


        delegate: Rectangle {
            id:delegate
            width: gView.cellWidth
            height: gView.cellHeight

            property bool hoveredOrClicked: false
            // border.color: (delegate.hoveredOrClicked ? "white" : "black")
            // border.width: 2


            Image {
                id: name
                anchors.fill: parent
                anchors.margins: 2
                fillMode: Image.PreserveAspectFit
                source: {
                    switch(index) {
                    case 0: return "qrc:/images/Kegel.png"
                    case 1: return "qrc:/images/Wuerfel.png"
                    case 2: return "qrc:/images/Zylinder.png"
                    case 3: return "qrc:/images/Pyramide.png"
                    case 4: return "qrc:/images/Prisma.png"
                    case 5: return "qrc:/images/Kugel.png"
                    }
                }

            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    root3dPopup.selection3d(index)
                    delegate.hoveredOrClicked = true
                }
                onEntered: {
                    delegate.hoveredOrClicked = true
                }
                onExited: {
                    delegate.hoveredOrClicked = false
                }
            }
        }
    }
}
