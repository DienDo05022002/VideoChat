#ifndef CONDUCTOR_H
#define CONDUCTOR_H

#include <QObject>
#include <QImage>
#include <QJsonObject>
#include <QObject>
#include <QVideoSink>
#include "api/media_stream_interface.h"
#include "rtc_base/thread.h"
#undef emit
#include "api/peer_connection_interface.h"
#define emit

#include "websocket_client.h"

class VideoRenderer;
class DummyCreateSessionDescriptionObserver;
class DummySetSessionDescriptionObserver;
class ClientConnectionObserver;

class Conductor : public QObject
{
    Q_OBJECT
public:
    explicit Conductor(QObject *parent = nullptr);
    ~Conductor();
    Q_INVOKABLE void startLogin(QString ip,int port);
    Q_INVOKABLE void signOut();

    Q_INVOKABLE void connectToPeer(int peerid);
    Q_INVOKABLE WebsocketClient* getWsClient();

    bool InitializePeerConnection();
    bool CreatePeerConnection();
    void DeletePeerConnection();
    void AddTracks();

    QVideoSink *localVideoSink() const;
    Q_INVOKABLE void setLocalVideoSink(QVideoSink *newLocalVideoSink);
    Q_INVOKABLE void resetLocalVideoSink();
    QVideoSink *remoteVideoSink() const;
    Q_INVOKABLE void setRemoteVideoSink(QVideoSink *newRemoteVideoSink);
    Q_INVOKABLE void resetRemoteVideoSink();

    void setServerIp(QString ip);
    void setPort(int port);
Q_SIGNALS:
    void freshPeerList();
    void newLocalVideoFrame(const QImage image);
    void switchToStreamingUi();
public Q_SLOTS:
    void sendToPeer(const QString& message);
    void setMyId(int myid);
    void onPeerConnectedOrDisconnected();
    void onMessageFromPeer(const QJsonObject& jsonObj);
    void onAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver);
private:

    bool loopback_ = false;
    std::unique_ptr<rtc::Thread> signaling_thread_;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory_;

    std::unique_ptr<VideoRenderer> local_renderer_;
    std::unique_ptr<VideoRenderer> remote_renderer_;
    ClientConnectionObserver *m_ClientConnectionObserver = nullptr;
    DummyCreateSessionDescriptionObserver *m_DummyCreateSessionDescriptionObserver = nullptr;

    QVideoSink *m_localVideoSink = nullptr;
    QVideoSink *m_remoteVideoSink = nullptr;

    WebsocketClient* m_client;
    QString m_serverIp;
    int m_port;
    int m_otherid;
    int m_myid;
};

#endif // CONDUCTOR_H
