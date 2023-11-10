import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: page
    width: 900
    height: 600
    Rectangle {
        id: loginPage
        anchors.fill: parent
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCente
        Label {
            id: titleLab1
            text: qsTr("Hi guys, Welcome to video-chat application with webrtc. To start using the application. Let's")
            font.family: "Helvetica"; font.pointSize: 15//; font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
            //anchors.bottom:colResgister.top
            //anchors.topMargin : 100
        }
        Label {
            id: titleLab2
            text: qsTr("CREATE AN ACCOUNT :)")
            font.pixelSize: 40
            font.family: "Arial Black"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top:titleLab1.bottom
            //anchors.topMargin : 100
        }

        ColumnLayout {
            id: colResgister
            anchors.centerIn: parent
            spacing: 10
            anchors {
                left: parent.left
                right: parent.right
                verticalCenter: parent.verticalCenter
            }

            RowLayout{
                Label{
                    text: qsTr("Username")
                    font.pixelSize: 16
                }
                Rectangle {
                    id: borderRect
                    width: 200
                    height: 27
                    color: "transparent"
                    border.color: "lightgray" // Màu viền
                    border.width: 0.5 // Độ dày của viền

                    TextInput {
                        id: usernameinput
                        anchors.fill: parent
                        text: qsTr(" ba dien")
                        clip: true
                        font.pixelSize: 18
                    }
                }
            }

            Button {

                id: usrinputbtn
                text: qsTr("Register")
                font.family: "Arial Black"
                font.pixelSize: 20
                anchors.topMargin: 100
                Layout.fillWidth: true
                Layout.fillHeight: true
                onClicked: {
                    if(usernameinput.text==="") return;
                    conductor.getWsClient().setUsername(usernameinput.text);
                    stackView.push(connectpage)
                    console.log(conductor.getWsClient().getUsername());
                }
            }
        }
    }
}
