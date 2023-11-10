import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts

//Item {
//}
Rectangle {
    Connections {
        target: mousearea
        function onClicked() {
            conductor.connectToPeer(Number(idlabel.text));
            console.log("connectto:")
            console.log(Number(idlabel.text));
        }
    }

    property alias idlabel: idlabel
    property alias namelabel: namelabel
    property alias mousearea: mousearea
    border.width: 2
    border.color:"#f2f2f2"
    color:"#ADD8E6"
    RowLayout {
        anchors.fill: parent
        Label {
            id: idlabel
            text: qsTr("id")
            font.pixelSize: 18
            font.family: "Arial Black"
            Layout.preferredHeight: parent.height
            Layout.preferredWidth: parent.width/3
        }
        Label {
            id: namelabel
            text: qsTr("name")
            font.pixelSize: 18
            Layout.preferredHeight: parent.height
            Layout.preferredWidth: parent.width/2
        }
    }
    MouseArea {
        id: mousearea
        anchors.fill: parent

    }
}
