import QtQuick
import "../js/ShapeIcons.js" as ShapeIcons

// One directed give/receive action (Inside puzzle's sort/distribute steps)
// or a final combine instruction, styled like StepCard (numbered, divider,
// tap-to-check-off) but as a single line instead of StepCard's 3-column
// swap table - that table shows every node's column whether or not it's
// relevant to the app's user, which is exactly the clutter this avoids.
Rectangle {
    id: root

    property int stepNumber: 1
    property string mode: "give" // "give" | "combine"
    // mode: "give"
    property string fromLabel: ""
    property string toLabel: ""
    property int symbol: 0
    // mode: "combine"
    property int symbolA: 0
    property int symbolB: 0
    property bool done: false

    color: "#161616"
    radius: 8
    border.color: root.done ? "#2e7d46" : "#333333"
    border.width: root.done ? 2 : 1
    opacity: root.done ? 0.55 : 1
    implicitWidth: 320
    implicitHeight: 48

    Behavior on opacity { NumberAnimation { duration: 120 } }
    Behavior on border.color { ColorAnimation { duration: 120 } }

    MouseArea {
        anchors.fill: parent
        onClicked: root.done = !root.done
    }

    Rectangle {
        width: 22
        height: 22
        radius: 11
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 6
        color: "#2e7d46"
        visible: root.done

        Text {
            anchors.centerIn: parent
            text: "✓"
            color: "#ffffff"
            font.bold: true
            font.pixelSize: 13
        }
    }

    Text {
        id: numberText
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.leftMargin: 14
        width: 26
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        text: root.stepNumber
        color: "#3b82f6"
        font.bold: true
        font.pixelSize: 20
    }

    Rectangle {
        id: numberDivider
        anchors.left: numberText.right
        anchors.leftMargin: 12
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 1
        color: "#333333"
    }

    // Content area right of the divider - the give/combine Row below is
    // centered WITHIN this (not the whole card), so it lands centered in
    // the space actually available to it instead of off to one side.
    Item {
        id: contentArea
        anchors.left: numberDivider.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        Row {
            anchors.centerIn: parent
            spacing: 8
            visible: root.mode === "give"

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("%1 gives").arg(root.fromLabel)
                color: "#dddddd"
                font.pixelSize: 13
            }
            Image {
                anchors.verticalCenter: parent.verticalCenter
                width: 20
                height: 20
                fillMode: Image.PreserveAspectFit
                source: ShapeIcons.iconSource(root.symbol)
            }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("to %1").arg(root.toLabel)
                color: "#dddddd"
                font.pixelSize: 13
            }
        }

        Row {
            anchors.centerIn: parent
            spacing: 8
            visible: root.mode === "combine"

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Combine")
                color: "#dddddd"
                font.pixelSize: 13
            }
            Image {
                anchors.verticalCenter: parent.verticalCenter
                width: 20
                height: 20
                fillMode: Image.PreserveAspectFit
                source: ShapeIcons.iconSource(root.symbolA)
            }
            Image {
                anchors.verticalCenter: parent.verticalCenter
                width: 20
                height: 20
                fillMode: Image.PreserveAspectFit
                source: ShapeIcons.iconSource(root.symbolB)
            }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("and leave the room")
                color: "#dddddd"
                font.pixelSize: 13
            }
        }
    }
}
