import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    title: qsTr("Peer List")
//    width: parent.width
//    height: parent.height
    property string myUsername: conductor.getWsClient().getUsername()
    Connections {
        target: conductor
        function onFreshPeerList() {
            console.log("fresh");
            listView.model.clear();
            for(let peer of conductor.getWsClient().getPeerList())
            {
                console.log("fresh:for loop");
                console.log(JSON.stringify(peer))
                listView.model.append(peer);
            }
        }
    }
    Connections {
        target:conductor
        function onSwitchToStreamingUi() {
            console.log("switchToStreamingUi");
            stackView.push(streamingpage);
        }
    }

    //**************Thanh tiêu đề dashboard**************
    Rectangle {
        id: barDashboard
        width: parent.width
        height: 60 // Điều chỉnh chiều cao của thanh bar
        color: "#ffffff" // Màu của thanh bar
        border.color: "#808080" // Màu viền
        border.width: 1
        Text {
            text: "Dashboard"
            font.pixelSize: 24
            font.family: "Arial Black"
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
        }

        Rectangle {
            id: iconUsername
            anchors.left:  parent.horizontalCenter
            anchors.leftMargin: 200
            height: 40
            width: 40
            radius: 50
            anchors.verticalCenter: parent.verticalCenter

            Image {
                source: "pics/account_username-25.png" // Thay đổi đường dẫn đến biểu tượng người dùng
                width: 40
                height: 40
                anchors.fill: parent
            }
        }
        Rectangle {
            id: recMyUsername
            anchors.left: iconUsername.right
            height: 40
            width: 100
            color: "lightgray"
            border.color: "black"
            border.width: 2.5
            radius: 10
            anchors.verticalCenter: parent.verticalCenter

            Text{
                text: myUsername
                font.pixelSize: 20
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

    }

    Rectangle{
        id: iconListhead
        anchors.top: barDashboard.bottom
        width: parent.width
        height: 50
        //color: "red"
        Rectangle {
            anchors.horizontalCenter:  iconListhead.horizontalCenter
            anchors.verticalCenter: iconListhead.verticalCenter
            height: 40
            width: 50
            radius: 200
            clip: true
            Image {
                source: "pics/group-users-icon.png" // Thay đổi đường dẫn đến biểu tượng người dùng
                width: 50
                height: 40
                anchors.leftMargin: 200
                //anchors.fill: parent
            }
        }
    }


    ListView {
        anchors.top: iconListhead.bottom
        id: listView
        anchors.fill: parent
        anchors.rightMargin: 20
        anchors.leftMargin: 20
        anchors.bottomMargin: 5
        anchors.topMargin: 120
        //header: listhead
        model: ListModel {}
        delegate:PeerDelegate {
            //anchors.topMargin:  200
            width: listView.width
            height: listView.height/15//20
            idlabel.text: model.id
            namelabel.text: model.name
        }
    }
}
