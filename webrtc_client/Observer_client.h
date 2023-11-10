#ifndef OBSERVER_CLIENT_H
#define OBSERVER_CLIENT_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QImage>
#include <QObject>
#include <QMutex>
#include <QDebug>
#include <QVideoFrameFormat>
#include <QVideoFrame>

#undef emit
#include "api/peer_connection_interface.h"
#define emit
#include "api/jsep.h"
#include "api/make_ref_counted.h"

//class Observer_client
//{
//public:
//    Observer_client();
//};
//************** ![1] Create **************
class DummyCreateSessionDescriptionObserver : public QObject,
                                               public webrtc::CreateSessionDescriptionObserver
{
    Q_OBJECT
public:
    explicit DummyCreateSessionDescriptionObserver(
        bool &loopback,
        rtc::scoped_refptr<webrtc::PeerConnectionInterface> &peer_connection,
        QObject *parent = nullptr);

Q_SIGNALS:
    //    void error(const QString errorString, const QString detailString = QString());

    //信令，发送消息给对端
    void sendMessageToPeer(const QString& message);
protected:
    void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
    void OnFailure(webrtc::RTCError error) override;

    void AddRef() const override {}
    rtc::RefCountReleaseStatus Release() const override {
        return rtc::RefCountReleaseStatus::kDroppedLastRef;}

private:
    bool &m_loopback;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> &m_peer_connection;
};
//************** ![1] End **************


//************** ![2] Set **************
class DummySetSessionDescriptionObserver
    : public webrtc::SetSessionDescriptionObserver {
public:
//    static DummySetSessionDescriptionObserver* Create() {
//        return new rtc::RefCountedObject<DummySetSessionDescriptionObserver>();
//    }
    static rtc::scoped_refptr<DummySetSessionDescriptionObserver> Create() {
        return rtc::make_ref_counted<DummySetSessionDescriptionObserver>();
    }
//    virtual void OnSuccess() { RTC_LOG(INFO) << __FUNCTION__; }
    virtual void OnSuccess() { RTC_LOG(LS_INFO) << __FUNCTION__; }
    virtual void OnFailure(webrtc::RTCError error) {
        RTC_LOG(LS_INFO) << __FUNCTION__ << " " << ToString(error.type()) << ": "
                      << error.message();
    }
};
//************** ![2] END **************


//************** ![3] Connect **************
class ClientConnectionObserver : public QObject,
                                     public webrtc::PeerConnectionObserver
{
    Q_OBJECT
public:
    explicit ClientConnectionObserver(
        bool &loopback,
        rtc::scoped_refptr<webrtc::PeerConnectionInterface> &peer_connection,
        QObject *parent = nullptr);

Q_SIGNALS:
    //    void error(const QString errorString, const QString detailString = QString());
    void addTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver);
    //信令，发送消息给对端
    void sendMessageToPeer(const QString message);
protected:
    void OnSignalingChange(
        webrtc::PeerConnectionInterface::SignalingState new_state) override {}

    void OnDataChannel(
        rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override {}

    void OnIceGatheringChange(
        webrtc::PeerConnectionInterface::IceGatheringState new_state) override {}

    void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
    void OnAddTrack(
        rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
        const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>&
            streams) override;
    void OnRemoveTrack(
        rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override;
private:
    bool &m_loopback;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> &m_peer_connection;
};
//************** ![3] END **************

#endif // OBSERVER_CLIENT_H
