#include "Observer_client.h"

//Observer_client::Observer_client()
//{

//}
//************** ![1] **************
DummyCreateSessionDescriptionObserver::DummyCreateSessionDescriptionObserver(
    bool &loopback,
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> &peer_connection,
    QObject *parent)
    : QObject{parent}, m_loopback(loopback), m_peer_connection(peer_connection)
{
    qDebug() << "DummyCreateSessionDescriptionObserver::DummyCreateSessionDescriptionObserver";
}

void DummyCreateSessionDescriptionObserver::OnSuccess(webrtc::SessionDescriptionInterface *desc)
{
qDebug() << "DummyCreateSessionDescriptionObserver::OnSuccess";
#ifndef QT_NO_DEBUG
    qInfo()<<Q_FUNC_INFO<< __LINE__ ;
#endif
    m_peer_connection->SetLocalDescription(
        DummySetSessionDescriptionObserver::Create().get(), desc);

    std::string sdp;
    desc->ToString(&sdp);

    // For loopback test. To save some connecting delay.
    if (m_loopback) {
        // Replace message type from "offer" to "answer"
        std::unique_ptr<webrtc::SessionDescriptionInterface> session_description =
            webrtc::CreateSessionDescription(webrtc::SdpType::kAnswer, sdp);
        m_peer_connection->SetRemoteDescription(
            DummySetSessionDescriptionObserver::Create().get(),
            session_description.release());
        return;
    }

    QJsonObject messageJsonObject;
    messageJsonObject["kind"] = QString(webrtc::SdpTypeToString(desc->GetType()));
    messageJsonObject["sdp"] = QString::fromStdString(sdp);
    QJsonDocument doc(messageJsonObject);
    Q_EMIT sendMessageToPeer(doc.toJson(QJsonDocument::Compact));
}

void DummyCreateSessionDescriptionObserver::OnFailure(webrtc::RTCError error)
{
qDebug() << "DummyCreateSessionDescriptionObserver::OnFailure";
#ifndef QT_NO_DEBUG
    qInfo()<<Q_FUNC_INFO<< __LINE__ ;
#endif
    //    RTC_LOG(LS_ERROR) << ToString(error.type()) << ": " << error.message();

    //    Q_EMIT this->error(tr("获取会话描述失败!"), error.message());
}
//************** ![1]END **************

//************** ![3] **************
ClientConnectionObserver::ClientConnectionObserver(
    bool &loopback,
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> &peer_connection,
    QObject *parent)
    : QObject{parent}, m_loopback(loopback), m_peer_connection(peer_connection)
{
    qDebug() << "ClientConnectionObserver::ClientConnectionObserver";
}

void ClientConnectionObserver::OnIceCandidate(const webrtc::IceCandidateInterface *candidate)
{
 qDebug() << "ClientConnectionObserver::OnIceCandidate";
#ifndef QT_NO_DEBUG
    qInfo()<<Q_FUNC_INFO<< __LINE__ ;
#endif
    //    RTC_LOG(LS_INFO) << __FUNCTION__ << " " << candidate->sdp_mline_index();
    // For loopback test. To save some connecting delay.
    if (m_loopback) {
        if (!m_peer_connection->AddIceCandidate(candidate)) {
            qDebug()<<"Failed to apply the received candidate";
            //        Q_EMIT error(tr("处理接收的candidate失败!"));
        }
        return;
    }

    std::string sdp;
    if (!candidate->ToString(&sdp)) {
        //      RTC_LOG(LS_ERROR) << "Failed to serialize candidate";
        //      Q_EMIT error(tr("接收的candidate,序列化失败!"));
        return;
    }
    QJsonObject messageJsonObject;
    messageJsonObject["sdpMid"] = QString::fromStdString(candidate->sdp_mid());
    messageJsonObject["sdpMLineIndex"] = candidate->sdp_mline_index();
    messageJsonObject["candidate"] = QString::fromStdString(sdp);
    qDebug()<<"messageJsonObject:"<<QJsonDocument(messageJsonObject);
    //不清楚还要如何处理？？？？应该发给对端处理？ ？？？？？？？？
    qDebug()<<"OnIceCandidate ! candidate"<<messageJsonObject;
    QJsonDocument doc(messageJsonObject);
    Q_EMIT sendMessageToPeer(doc.toJson(QJsonDocument::Compact));
}

void ClientConnectionObserver::OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver, const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface> > &streams)
{
    qDebug()<<"ClientConnectionObserver::OnAddTrack";
    receiver->track().release();
    Q_EMIT addTrack(receiver);
}

void ClientConnectionObserver::OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver)
{
    qDebug()<<"ClientConnectionObserver::OnRemoveTrack";
    auto* track = reinterpret_cast<webrtc::MediaStreamTrackInterface*>(receiver->track().release());
    track->Release();
}
//************** ![3] END **************
