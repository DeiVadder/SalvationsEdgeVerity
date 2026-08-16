import QtQuick

// Full-window modal help overlay - "?" icon in main.qml's header opens
// this. Custom-built (not QtQuick.Controls Dialog) to match the rest of
// the app's dark, hand-styled chrome instead of native dialog styling.
Item {
    id: root
    anchors.fill: parent
    visible: false
    z: 1000

    function open() { root.visible = true }
    function close() { root.visible = false }

    // Dimmed backdrop - tap outside the panel to close.
    Rectangle {
        anchors.fill: parent
        color: "#000000"
        opacity: 0.6

        MouseArea {
            anchors.fill: parent
            onClicked: root.close()
        }
    }

    Rectangle {
        id: panel
        anchors.centerIn: parent
        width: Math.min(parent.width - 40, 520)
        height: Math.min(parent.height - 40, 620)
        radius: 10
        color: "#161616"
        border.color: "#333333"

        // Swallow clicks so they don't fall through to the backdrop.
        MouseArea {
            anchors.fill: parent
        }

        Row {
            id: header
            x: 20
            y: 16
            width: parent.width - 40
            height: 28

            Text {
                text: qsTr("How to use this app")
                color: "#ffffff"
                font.pixelSize: 17
                font.bold: true
                width: parent.width - closeButton.width
                elide: Text.ElideRight
            }

            Rectangle {
                id: closeButton
                width: 28
                height: 28
                radius: 6
                color: "#2a2a2a"
                border.color: "#444444"

                Text {
                    anchors.centerIn: parent
                    text: "✕"
                    color: "#dddddd"
                    font.pixelSize: 13
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: root.close()
                }
            }
        }

        Flickable {
            x: 20
            y: header.y + header.height + 12
            width: parent.width - 40
            height: parent.height - y - 16
            contentWidth: width
            contentHeight: helpText.implicitHeight
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            Text {
                id: helpText
                width: parent.width
                wrapMode: Text.WordWrap
                color: "#cccccc"
                font.pixelSize: 13
                lineHeight: 1.3
                textFormat: Text.RichText
                text: {
                    var modeHeading = qsTr("Modes")
                    var modeBody = qsTr("Outside / Inside / Ghosts switches between the outside dissection puzzle, the inside solo-room puzzle, and the ghost phase helper. Reset only clears the current mode.")
                    var outsideHeading = qsTr("Outside puzzle")
                    var outsideBody = qsTr("Pick each statue's inside 2D symbol and outside 3D shape. Once 2 of the 3 are picked, the app guesses the 3rd one (amber border) - tap any slot to override the guess. Hover a 3D shape to see the 2 base symbols it's made of.")
                    var challengeHeading = qsTr("Challenge Mode (Outside)")
                    var challengeBody = qsTr("Enable Challenge Mode to track “Varied Geometry”: tap each 3D shape you actually escape with. A shape dims once used and can't repeat until all 6 have been used. “New encounter” clears this tracker only - it's not the same as the top Reset button.")
                    var solutionHeading = qsTr("Solution")
                    var solutionBody = qsTr("Switch to the Solution tab (or view it side-by-side on wide windows) to see the computed swap sequence and “Copy for in-game chat”.")
                    var insideHeading = qsTr("Inside puzzle")
                    var insideBody = qsTr("Pick each player's own 2D symbol, then mark which statue you are. Enter your own wall (2 symbols) to see what you personally give away this round - give any symbol that isn't yours to whichever player owns it, wait for your teammates, then distribute. In Challenge Mode, also enter the outside team's 3 called escape shapes so distribute targets those instead of the default split. The Fast shortcut is coming back later. UNVERIFIED against real gameplay - sanity-check it before trusting it in a live run.")
                    var ghostsHeading = qsTr("Ghosts")
                    var ghostsBody = qsTr("Manual 6-player roster: name, class, and a ghost or fashion note per player. “Copy ghost callouts” copies a summary for chat. No Bungie API involved - no key needed, nothing sent anywhere.")
                    var languageHeading = qsTr("Language")
                    var languageBody = qsTr("The flag icons in the header switch the whole app between English and German instantly.")

                    return "<b>" + modeHeading + "</b><br>" + modeBody + "<br><br>"
                         + "<b>" + outsideHeading + "</b><br>" + outsideBody + "<br><br>"
                         + "<b>" + challengeHeading + "</b><br>" + challengeBody + "<br><br>"
                         + "<b>" + solutionHeading + "</b><br>" + solutionBody + "<br><br>"
                         + "<b>" + insideHeading + "</b><br>" + insideBody + "<br><br>"
                         + "<b>" + ghostsHeading + "</b><br>" + ghostsBody + "<br><br>"
                         + "<b>" + languageHeading + "</b><br>" + languageBody
                }
            }
        }
    }
}
