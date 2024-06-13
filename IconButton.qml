import QtQuick 2.15

Rectangle {
    id: root
    objectName: parent.objectName + "IconBtn"

    signal clicked()

    property string source: ""
    property string altSource: source
    property int imgMargin: 0
    property int imageVOffset: 0

    property color colorPressed: colorNormal/*"lightgrey"*/
    property color colorNormal: "transparent"
    color: mArea.pressed ? colorPressed : colorNormal
    radius: height /10

    readonly property alias imageHeight: img.height

    Image {
        id:img
        anchors.centerIn: parent
        anchors.verticalCenterOffset: imageVOffset
        width: Math.min(root.width, root.height) - imgMargin
        height: width

        source: mArea.pressed ? altSource : root.source
        fillMode: Image.PreserveAspectFit
        mipmap: true

    }

    MouseArea {
        id:mArea
        objectName: root.objectName + "MouseArea"

        anchors.fill: parent

        onClicked: root.clicked()
    }
}
