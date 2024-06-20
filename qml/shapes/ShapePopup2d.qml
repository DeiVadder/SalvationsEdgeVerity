import QtQuick 2.15

Rectangle {
    id:root2dPopup

    signal selected(var index)

    GridView {
        id:gView
        anchors.fill: parent
        model: 3

        cellWidth: width / 3
        cellHeight: height


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
                    case 0: return "qrc:/images/Dreieck.png"
                    case 1: return "qrc:/images/Viereck.png"
                    case 2: return "qrc:/images/Kreis.png"
                    }
                }

            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    root2dPopup.selected(index + 1)
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
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        text: qsTr("Inside symbol")
        font.pixelSize: fontNormal
        topPadding: 5
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
    }

}
