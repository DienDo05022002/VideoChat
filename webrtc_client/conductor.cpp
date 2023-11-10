#include "conductor.h"
#include <QVideoFrameFormat>
#include <QVideoFrame>
#include <QJsonParseError>

#include "data_socket.h"
#include "Observer_client.h"
#include "websocket_client.h"
#include "video_renderer.h"
#include "capturer_track_source.h"

#include "p2p/base/port_allocator.h"
#include "pc/video_track_source.h"
#include "rtc_base/checks.h"
#include "rtc_base/rtc_certificate_generator.h"
#include "test/vcm_capturer.h"
#include "absl/memory/memory.h"
#include "absl/types/optional.h"
#include "api/audio/audio_mixer.h"
#include "api/audio_codecs/audio_decoder_factory.h"
#include "api/audio_codecs/audio_encoder_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/audio_options.h"
#include "api/create_peerconnection_factory.h"
#include "api/rtp_sender_interface.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "modules/audio_device/include/audio_device.h"
#include "modules/audio_processing/include/audio_processing.h"
#include "modules/video_capture/video_capture.h"
#include "modules/video_capture/video_capture_factory.h"

#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/video_decoder_factory_template.h"
#include "api/video_codecs/video_decoder_factory_template_dav1d_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_libvpx_vp8_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_libvpx_vp9_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_open_h264_adapter.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "api/video_codecs/video_encoder_factory_template.h"
#include "api/video_codecs/video_encoder_factory_template_libaom_av1_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_libvpx_vp8_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_libvpx_vp9_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_open_h264_adapter.h"


Conductor::Conductor(QObject *parent)
    : QObject{parent}
{
    m_client=new WebsocketClient();
    connect(m_client,&WebsocketClient::peerConnectedOrDisconnected,this,&Conductor::onPeerConnectedOrDisconnected);
    connect(m_client,&WebsocketClient::loginSuccess,this,&Conductor::freshPeerList);
    connect(m_client,&WebsocketClient::sendId,this,&Conductor::setMyId);
    connect(m_client,&WebsocketClient::messageFromPeer,this,&Conductor::onMessageFromPeer);
}

Conductor::~Conductor()
{
    if(m_client) {
        m_client->deleteLater();
        m_client=nullptr;
    }
    if(m_ClientConnectionObserver != nullptr)
    {
        m_ClientConnectionObserver->deleteLater();
        m_ClientConnectionObserver = nullptr;
    }
    if(m_DummyCreateSessionDescriptionObserver != nullptr)
    {
        m_DummyCreateSessionDescriptionObserver->deleteLater();
        m_DummyCreateSessionDescriptionObserver = nullptr;
    }
}

void Conductor::startLogin(QString ip, int port)
{
    if(m_client->isConnected()) return;
    m_serverIp=ip;
    m_port=port;
    m_client->startLogin(ip,port);
}

void Conductor::signOut()
{
    m_client->signOut();
}

void Conductor::connectToPeer(int peerid)
{
    if (peer_connection_.get()) {
        qDebug()<<"Error： We only support connecting to one peer at a time";
        return;
    }
    if (InitializePeerConnection()) {
        m_otherid = peerid;
        Q_EMIT switchToStreamingUi();
        peer_connection_->CreateOffer(
            m_DummyCreateSessionDescriptionObserver, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
    } else {
        qDebug()<<"Error: Failed to initialize PeerConnection";
    }
}

void Conductor::sendToPeer(const QString& message)
{
    QJsonParseError error;
    QJsonDocument jsonDocument=QJsonDocument::fromJson(message.toUtf8(),&error);
    if(error.error||!jsonDocument.isObject()) {
        qDebug()<<"Receive a message which can not be converted to JSON format";
        return;
    }
    QJsonObject messagejsonObj=jsonDocument.object();
    QJsonObject jsonObj;
    jsonObj.insert("type","message");
    jsonObj.insert("from",m_myid);
    jsonObj.insert("to",m_otherid);
    jsonObj.insert("message",messagejsonObj);
    QJsonDocument doc(jsonObj);
    m_client->sendToPeer(doc.toJson(QJsonDocument::Compact));
}

bool Conductor::InitializePeerConnection()
{
    if(peer_connection_factory_) {
        qDebug()<<"Error: peer_connection_factory is not null";
        return false;
    }
    else if(peer_connection_) {
        qDebug()<<"Error： peer_connection is not null,we only support connecting to one peer at a time";
        return false;
    }
    if (!signaling_thread_.get()) {
        signaling_thread_ = rtc::Thread::CreateWithSocketServer();
        signaling_thread_->Start();
    }
    peer_connection_factory_ = webrtc::CreatePeerConnectionFactory(
        nullptr /* network_thread */, nullptr /* worker_thread */,
        signaling_thread_.get(), nullptr /* default_adm */,
        webrtc::CreateBuiltinAudioEncoderFactory(),
        webrtc::CreateBuiltinAudioDecoderFactory(),
        //webrtc::CreateBuiltinVideoEncoderFactory(),
        //webrtc::CreateBuiltinVideoDecoderFactory(),
        std::make_unique<webrtc::VideoEncoderFactoryTemplate<
            webrtc::LibvpxVp8EncoderTemplateAdapter,
            webrtc::LibvpxVp9EncoderTemplateAdapter,
            webrtc::OpenH264EncoderTemplateAdapter,
            webrtc::LibaomAv1EncoderTemplateAdapter>>(),
        std::make_unique<webrtc::VideoDecoderFactoryTemplate<
            webrtc::LibvpxVp8DecoderTemplateAdapter,
            webrtc::LibvpxVp9DecoderTemplateAdapter,
            webrtc::OpenH264DecoderTemplateAdapter,
            webrtc::Dav1dDecoderTemplateAdapter>>(),
        nullptr /* audio_mixer */,
        nullptr /* audio_processing */);
    if (!peer_connection_factory_) {
        qDebug()<<"Error: Failed to initialize PeerConnectionFactory";
        DeletePeerConnection();
        return false;
    }
    if(m_ClientConnectionObserver == nullptr)
    {
        m_ClientConnectionObserver =
            new ClientConnectionObserver(loopback_, peer_connection_);
        connect(m_ClientConnectionObserver, &ClientConnectionObserver::sendMessageToPeer,
                this, &Conductor::sendToPeer);
        connect(m_ClientConnectionObserver,&ClientConnectionObserver::addTrack,this,&Conductor::onAddTrack);

    }
    if (!CreatePeerConnection()) {
        qDebug()<<"Error: CreatePeerConnection failed";
        DeletePeerConnection();
        return false;
    }
    if(m_DummyCreateSessionDescriptionObserver == nullptr)
    {
        m_DummyCreateSessionDescriptionObserver =
            new DummyCreateSessionDescriptionObserver(loopback_, peer_connection_);
        connect(m_DummyCreateSessionDescriptionObserver, &DummyCreateSessionDescriptionObserver::sendMessageToPeer,
                this, &Conductor::sendToPeer);
    }
    AddTracks();
    return peer_connection_ != nullptr;
}

bool Conductor::CreatePeerConnection()
{
    if(peer_connection_) {
        qDebug()<<"Error： peer_connection is not null,we only support connecting to one peer at a time";
        return false;
    }
    webrtc::PeerConnectionInterface::RTCConfiguration config;
    config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
    //    webrtc::PeerConnectionInterface::IceServer server;
    //    server.uri = QString("stun:stun.l.google.com:19302").toLocal8Bit().data();
    //    config.servers.push_back(server);

    webrtc::PeerConnectionDependencies pc_dependencies(m_ClientConnectionObserver);
    auto error_or_peer_connection =
        peer_connection_factory_->CreatePeerConnectionOrError(
            config, std::move(pc_dependencies));
    if (error_or_peer_connection.ok()) {
        peer_connection_ = std::move(error_or_peer_connection.value());
    }
    return peer_connection_ != nullptr;
}

void Conductor::DeletePeerConnection()
{
    peer_connection_ = nullptr;
    peer_connection_factory_ = nullptr;
    m_otherid = -1;
    loopback_=false;
}

void Conductor::AddTracks()
{
    if (!peer_connection_->GetSenders().empty()) {
        return;  // Already added tracks.
    }
    const char kAudioLabel[] = "audio_label";
    const char kStreamId[] = "stream_id";
    rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
        peer_connection_factory_->CreateAudioTrack(
            kAudioLabel,
            peer_connection_factory_->CreateAudioSource(cricket::AudioOptions())
                .get()));
    auto result_or_error = peer_connection_->AddTrack(audio_track, {kStreamId});
    if (!result_or_error.ok()) {
        qDebug()<<"Error: Failed to add audio track to PeerConnection";
        //        return;
    }
    const char kVideoLabel[] = "video_label";
    rtc::scoped_refptr<CapturerTrackSource> video_device =
        CapturerTrackSource::create();
    if (video_device) {
        rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track_(
            peer_connection_factory_->CreateVideoTrack(kVideoLabel,
                                                       video_device.get()));
        local_renderer_.reset(new VideoRenderer(video_track_.get()));
        connect(local_renderer_.get(), &VideoRenderer::newVideoFrame,
                this,[this](const QImage image){
                    //                    Q_EMIT newLocalVideoFrame(image);
                    if(m_localVideoSink == nullptr)
                        return;

                    QVideoFrameFormat frameFormat(image.size(), QVideoFrameFormat::Format_ABGR8888);
                    QVideoFrame f(frameFormat);
                    if (f.map(QVideoFrame::WriteOnly)) {
                        memcpy(f.bits(0), image.bits(), image.sizeInBytes());
                        f.setStartTime(0);
                        f.unmap();
                        m_localVideoSink->setVideoFrame(f);
                    }
                });
        result_or_error = peer_connection_->AddTrack(video_track_, {kStreamId});
        if (!result_or_error.ok()) {
            qDebug()<<"Error: Failed to add video track to PeerConnection";
            //        return;
        }
    }else {
        qDebug()<<"Error: OpenVideoCaptureDevice failed";
        //        return;
    }
}

QVideoSink *Conductor::localVideoSink() const
{
    return m_localVideoSink;
}

void Conductor::setLocalVideoSink(QVideoSink *newLocalVideoSink)
{
    if (m_localVideoSink == newLocalVideoSink)
        return;
    m_localVideoSink = newLocalVideoSink;
}

void Conductor::resetLocalVideoSink()
{
    setLocalVideoSink(nullptr);
}

QVideoSink *Conductor::remoteVideoSink() const
{
    return m_remoteVideoSink;
}

void Conductor::setRemoteVideoSink(QVideoSink *newRemoteVideoSink)
{
    if (m_remoteVideoSink == newRemoteVideoSink)
        return;
    m_remoteVideoSink = newRemoteVideoSink;
}

void Conductor::resetRemoteVideoSink()
{
    setRemoteVideoSink(nullptr);
}
WebsocketClient* Conductor::getWsClient()
{
    return m_client;
}

void Conductor::setServerIp(QString ip)
{
    m_serverIp=ip;
}

void Conductor::setPort(int port)
{
    m_port=port;
}

void Conductor::setMyId(int myid)
{
    m_myid=myid;
}

void Conductor::onPeerConnectedOrDisconnected()
{
    Q_EMIT freshPeerList();
}

void Conductor::onMessageFromPeer(const QJsonObject& jsonObj)
{
    qDebug()<<"onMessageFromPeer";
    if(m_otherid==-1) {
        qDebug()<<"peer id is invalid";
        return;
    }
    int otherId=jsonObj.value("from").toInt();
    qDebug()<< "peer id is:"<< otherId;
    if(!peer_connection_.get()) {
        qDebug()<< "peer_connection_ is null";
        m_otherid=otherId;
        if(!InitializePeerConnection()) {
            signOut();
            qDebug()<<"InitializePeerConnection Fail";
            return;}
        else {
            qDebug()<<"InitializePeerConnection Success";
            Q_EMIT switchToStreamingUi();
        }
    }
    else if(m_otherid!=otherId) {
        qDebug()<<"m_otherid!=otherId";
        return;
    }
    QJsonObject messageObj=jsonObj.value("message").toObject();
    QString kind=messageObj.value("kind").toString();
    qDebug()<<"kind:"<<kind;
    if(!kind.isEmpty()) {
        qDebug()<<"kind is not Empty()";
        absl::optional<webrtc::SdpType> type_maybe =
            webrtc::SdpTypeFromString(kind.toLocal8Bit().data());
        if (!type_maybe) {
            qDebug()<< "Unknown SDP type:"<< kind;
            return;
        }
        webrtc::SdpType type = *type_maybe;
        QString sdp=messageObj.value("sdp").toString();
        webrtc::SdpParseError error;
        std::unique_ptr<webrtc::SessionDescriptionInterface> session_description =
            webrtc::CreateSessionDescription(type, sdp.toLocal8Bit().data(), &error);
        if (!session_description) {
            qDebug() << "Can't parse received session description message SdpParseError was: "<< error.description;
            return;
        }
        peer_connection_->SetRemoteDescription(
            DummySetSessionDescriptionObserver::Create().get(),
            session_description.release());
        if (type == webrtc::SdpType::kOffer) {                                                          //sdp
            peer_connection_->CreateAnswer(
                m_DummyCreateSessionDescriptionObserver, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
        }
    }
    else {                                                                                          //ice condidate
        std::string sdp_mid=messageObj.value("sdpMid").toString().toLocal8Bit().data();
        int sdp_mlineindex =0;
        if(!messageObj.value("sdpMLineIndex").isNull()) {
            sdp_mlineindex=messageObj.value("sdpMLineIndex").toInt();
        }
        std::string sdp=messageObj.value("candidate").toString().toLocal8Bit().data();
        webrtc::SdpParseError error;
        //webrtc::IceCandidateInterface
        std::unique_ptr<webrtc::IceCandidateInterface> candidate(webrtc::CreateIceCandidate(sdp_mid, sdp_mlineindex, sdp, &error));
        if (!candidate) {
            qDebug()<< "Can't parse received candidate message.SdpParseError was: "<< error.description;
            return;
        }
        if (!peer_connection_->AddIceCandidate(candidate.get())) {
            qDebug()<< "Failed to apply the received candidate";
            return;
        }
        qDebug()<< "Received candidate :" << QJsonDocument(messageObj);
    }
}

void Conductor::onAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver)
{
    qDebug()<<"onAddTrack";
    auto* track = reinterpret_cast<webrtc::MediaStreamTrackInterface*>(receiver->track().release());
    if(!track) {
        qDebug()<<"MediaStreamTrackInterface 的问题";
        return;
    }
    if (track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind) {
        auto* video_track = static_cast<webrtc::VideoTrackInterface*>(track);
        if(!video_track) {
            qDebug()<<"VideoTrackInterface 的问题";
            return;
        }
        remote_renderer_.reset(new VideoRenderer(video_track));
        if(!remote_renderer_) {
            qDebug()<<"VideoRenderer 的问题";
            return;
        }
        connect(remote_renderer_.get(), &VideoRenderer::newVideoFrame,
                this,[this](const QImage image){
                    if(m_remoteVideoSink == nullptr)
                        return;

                    QVideoFrameFormat frameFormat(image.size(), QVideoFrameFormat::Format_ABGR8888);
                    QVideoFrame f(frameFormat);
                    if (f.map(QVideoFrame::WriteOnly)) {
                        memcpy(f.bits(0), image.bits(), image.sizeInBytes());
                        f.setStartTime(0);
                        f.unmap();
                        m_remoteVideoSink->setVideoFrame(f);
                    }
                });
    }
    track->Release();
}
