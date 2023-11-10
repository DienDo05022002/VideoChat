#ifndef CAPTURERTRACKSOURCE_H
#define CAPTURERTRACKSOURCE_H

#undef emit
#include "pc/video_track_source.h"
#include "test/vcm_capturer.h"
#define emit
#include <QObject>

class CapturerTrackSource : public QObject, public webrtc::VideoTrackSource
{
    Q_OBJECT
public:
    explicit CapturerTrackSource(QObject *parent = nullptr) = delete;

    static rtc::scoped_refptr<CapturerTrackSource> create();

    //signals:
protected:
    explicit CapturerTrackSource(
        std::unique_ptr<webrtc::test::VcmCapturer> capturer)
        : VideoTrackSource(/*remote=*/false), capturer_(std::move(capturer)) {}
private:
    rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
        return capturer_.get();
    }
    std::unique_ptr<webrtc::test::VcmCapturer> capturer_;
};

#endif // CAPTURERTRACKSOURCE_H
