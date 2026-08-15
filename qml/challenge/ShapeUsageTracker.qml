import QtQuick
import "../js/ShapeIcons.js" as ShapeIcons

// "Varied Geometry" challenge helper: 6 tappable shape slots, dimmed once
// used. Tap = manually confirm a shape was actually used to escape this
// encounter (not auto-derived - see EncounterProgress's header comment for
// why).
Rectangle {
    id: root

    property var progress
    readonly property var allShapes: [4, 5, 6, 7, 8, 9] // Kegel..Kugel

    visible: progress !== null && progress !== undefined && progress.challengeModeEnabled
    color: "#0d0d0d"
    radius: 10
    border.color: "#2a2a2a"
    implicitHeight: content.implicitHeight + 24

    Column {
        id: content
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 12
        spacing: 8

        Row {
            spacing: 10

            Text {
                text: "Varied Geometry"
                color: "#ffffff"
                font.pixelSize: 14
                font.bold: true
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                text: root.progress ? (root.progress.usedShapeCount + " / 6 used") : ""
                color: "#999999"
                font.pixelSize: 12
                anchors.verticalCenter: parent.verticalCenter
            }

            Rectangle {
                width: 80
                height: 24
                radius: 5
                color: "#2a2a2a"
                border.color: "#444444"
                visible: root.progress !== null && root.progress !== undefined

                Text {
                    anchors.centerIn: parent
                    text: "New encounter"
                    color: "#cccccc"
                    font.pixelSize: 10
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: if (root.progress) root.progress.resetEncounter()
                }
            }
        }

        Row {
            spacing: 10

            Repeater {
                model: root.allShapes

                delegate: Rectangle {
                    id: cell
                    required property var modelData

                    readonly property bool available: root.progress ? root.progress.isShapeAvailable(cell.modelData) : true

                    width: 48
                    height: 48
                    radius: 6
                    color: cell.available ? "#1a1a1a" : "#0a0a0a"
                    border.color: cell.available ? "#3a3a3a" : "#3b82f6"
                    opacity: cell.available ? 1.0 : 0.45

                    Image {
                        anchors.centerIn: parent
                        width: parent.width - 14
                        height: parent.height - 14
                        source: ShapeIcons.iconSource(cell.modelData)
                        fillMode: Image.PreserveAspectFit
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: if (root.progress) root.progress.markShapeUsed(cell.modelData)
                    }
                }
            }
        }
    }
}
