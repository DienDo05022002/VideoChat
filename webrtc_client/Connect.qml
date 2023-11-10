import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: page
    Rectangle {
        anchors.fill: parent
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        Label {
            id: titleLab1
            text: qsTr("Connect to Server")
            font.family: "Helvetica"; font.pointSize: 40//; font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: rowConnect.top
            anchors.bottomMargin: 100
        }
        RowLayout {
            id: rowConnect
            anchors.centerIn: parent
            Label {
                text: qsTr("IP:")
                font.pixelSize: 25
                color: "#343b74"
                Layout.preferredHeight: page.height / 15
                Layout.preferredWidth: page.width / 30
            }
            Rectangle {
                width: 100
                height: 38
                color: "transparent"
                border.color: "lightgray" // Màu viền
                border.width: 0.5 // Độ dày của viền
                anchors.rightMargin: 5

                TextInput {
                    id: ipinput
                    anchors.fill: parent
                    anchors.margins: 3
                    text: qsTr("127.0.0.1")
                    clip: true
                    font.pixelSize: 25
                }
            }
            Label {
                text: qsTr("PORT:")
                font.pixelSize: 25
                color: "#343b74"
                Layout.preferredHeight: page.height / 15
                Layout.preferredWidth: page.width / 15
            }
            Rectangle {
                width: 62
                height: 38
                color: "transparent"
                border.color: "lightgray" // Màu viền
                border.width: 0.5 // Độ dày của viền
                anchors.rightMargin: 5

                TextInput {
                    id: portinput
                    anchors.fill: parent
                    anchors.margins: 3
                    text: qsTr("4567")
                    clip: true
                    font.pixelSize: 25
                }
            }
        }
        Button {
            id: connectbtn
            text: qsTr("Connect")
            anchors.top: rowConnect.bottom
            anchors.topMargin: 50
            font.family: "Arial Black"
            font.pixelSize: 20
            anchors.horizontalCenter: parent.horizontalCenter
            width: 150
            height: 30

            onClicked: {
                conductor.startLogin(ipinput.text,Number(portinput.text));
                stackView.push(peerpage)
            }
        }
    }

}
