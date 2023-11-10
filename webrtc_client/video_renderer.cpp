#include "video_renderer.h"
#include "api/video/i420_buffer.h"
#include "third_party/libyuv/include/libyuv.h"

//#include <synchapi.h>
#include <QMutexLocker>
#include <QDebug>

VideoRenderer::VideoRenderer(webrtc::VideoTrackInterface* track_to_render,
                                                   QObject *parent)
    : QObject{parent}, rendered_track_(track_to_render)
{
    qDebug() << "VideoRenderer::VideoRenderer";
    QMutexLocker mutexLocker(&m_mutex);
    rendered_track_->AddOrUpdateSink(this, rtc::VideoSinkWants());
}

VideoRenderer::~VideoRenderer()
{
    qDebug() << "VideoRenderer::~VideoRenderer";
    QMutexLocker mutexLocker(&m_mutex);
    rendered_track_->RemoveSink(this);
}

void VideoRenderer::OnFrame(const webrtc::VideoFrame &videoFrame)
{
    qDebug() << "VideoRenderer::OnFrame";
    QMutexLocker mutexLocker(&m_mutex);
    rtc::scoped_refptr<webrtc::I420BufferInterface> buffer(
        videoFrame.video_frame_buffer()->ToI420());
    if (videoFrame.rotation() != webrtc::kVideoRotation_0) {
        buffer = webrtc::I420Buffer::Rotate(*buffer, videoFrame.rotation());
    }

    const int length =buffer->width()* buffer->height()*4;
    //    qDebug()<<"buffer->width()"<<buffer->width()<<"buffer->height()"<<buffer->height();
    uchar *imageBuffer=new uchar[length];


    libyuv::I420ToRGBA(buffer->DataY(), buffer->StrideY(), buffer->DataU(),
                       buffer->StrideU(), buffer->DataV(), buffer->StrideV(),
                       imageBuffer,
                       buffer->width()*4,
                       buffer->width(), buffer->height());

    QImage image(imageBuffer,buffer->width(), buffer->height(), QImage::Format_RGBA8888);
    Q_EMIT newVideoFrame(image.copy());
    delete[] imageBuffer;
}
