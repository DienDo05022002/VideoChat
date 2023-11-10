#ifndef VIDEORENDERER_H
#define VIDEORENDERER_H

#include "api/media_stream_interface.h"
#include "api/video/video_frame.h"
#include <QImage>
#include <QObject>
#include <QMutex>

class VideoRenderer : public QObject,
                      public rtc::VideoSinkInterface<webrtc::VideoFrame>
{
    Q_OBJECT
public:
    explicit VideoRenderer(webrtc::VideoTrackInterface* track_to_render,
                                      QObject *parent = nullptr);
    ~VideoRenderer();

    //Process received frames
    void OnFrame(const webrtc::VideoFrame& videoFrame) override;
Q_SIGNALS:
    void newVideoFrame(const QImage image);
private:
    rtc::scoped_refptr<webrtc::VideoTrackInterface> rendered_track_;

    QMutex m_mutex;
};

#endif // VIDEORENDERER_H
