import QtQuick 2.15

Rectangle {
    id:root3dPopup
    signal selection3d(var index)


    GridView {
        id:gView
        anchors.fill: parent
        anchors.topMargin: description.height * 1.2
        model: 6

        cellWidth: width / 2
        cellHeight: height / 3


        delegate: Rectangle {
            id:delegate
            width: gView.cellWidth
            height: gView.cellHeight

            property bool hoveredOrClicked: false


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
    Text {
        id:description
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        height: contentHeight * 1.2
        text: qsTr("Outside symbol1")
        font.pixelSize: fontNormal
        topPadding: 5
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignTop
    }
}
