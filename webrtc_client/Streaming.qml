import QtQuick 2.15
import QtQuick.Controls
import QtMultimedia
import QtQuick.Layouts

Item {
    id: page
    property alias videoOutput_local: videoOutput_local
    property alias videoOutput_remote: videoOutput_remote

    Component.onCompleted: {
        conductor.setLocalVideoSink(videoOutput_local.videoSink);
        conductor.setRemoteVideoSink(videoOutput_remote.videoSink);
    }
    Component.onDestruction: {
        conductor.resetLocalVideoSink();
        conductor.resetRemoteVideoSink();
    }

    ColumnLayout {
        anchors.fill: parent
        VideoOutput {
            id: videoOutput_local
            Layout.preferredHeight: page.height/2
            Layout.preferredWidth: page.width
            Label {
                font.pixelSize: 15
                text: qsTr("Caller")
                anchors.left: parent.left
                anchors.top: parent.top
            }
        }
        VideoOutput {
            id: videoOutput_remote
            Layout.preferredHeight: page.height/2
            Layout.preferredWidth: page.width
            Label {
                font.pixelSize: 15
                text: qsTr("Receiver")
                anchors.left: parent.left
                anchors.top: parent.top
            }
        }
    }

}
