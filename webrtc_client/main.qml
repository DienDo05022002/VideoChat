import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    width: 900
    height: 600
    visible: true
    title: qsTr("Video-Chat")

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: Register {}
    }

    Component {
        id: peerpage
        PeerList {
            width: stackView.width
            height: stackView.height}
    }
    Component {
        id: connectpage
        Connect {
            width: stackView.width
            height: stackView.height}
    }
    Component {
        id: streamingpage
        Streaming {
            width: stackView.width
            height: stackView.height}
    }
}
