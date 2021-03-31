#ifndef VIDEO_PIXMAP_WIDGET_H
#define VIDEO_PIXMAP_WIDGET_H

#include <QWidget>
#include <QTimer>
#include <mutex>
#include "otl_utils.h"
#include "bm_face_sdk.h"

namespace Ui {
class video_pixmap_widget;
}

#define BM_UPDATE_VIDEO (QEvent::Type)(QEvent::User + 1)

class IVideoDrawer {
public:
    virtual ~IVideoDrawer(){}
    virtual int frame_width() = 0;
    virtual int frame_height() = 0;
    virtual int draw_frame(const AVFrame *frame, bool bUpdate=true) = 0;
    virtual int draw_rect(const fdrtsp::FaceRectVector& vct_rect, const std::vector<fdrtsp::FaceRecognitionInfo> &vct_label, bool bUpdate=true) = 0;
    virtual int clear_frame() = 0;
};

class video_pixmap_widget : public QWidget, public IVideoDrawer
{
    Q_OBJECT

public:
    explicit video_pixmap_widget(QWidget *parent = nullptr);
    ~video_pixmap_widget();

    virtual int frame_width() override;
    virtual int frame_height() override;
    virtual int draw_frame(const AVFrame *frame, bool bUpdate=true) override;
    virtual int draw_rect(const fdrtsp::FaceRectVector& vct_rect, const std::vector<fdrtsp::FaceRecognitionInfo> &vct_label, bool bUpdate=true) override;
    int clear_frame() override;

protected:
    void paintEvent(QPaintEvent *event) override;
    bool event(QEvent *e) override;
protected slots:
    void onRefreshTimeout();
private:
    unsigned char* avframe_to_rgb32(const AVFrame *frame);

    Ui::video_pixmap_widget *ui;
    AVFrame *m_avframe {0};
    fdrtsp::FaceRectVector m_vct_face_rect;
    std::vector<fdrtsp::FaceRecognitionInfo> m_vct_label;
    QTimer *m_refreshTimer;
    std::mutex m_syncLock;
    int m_roi_heatbeat{0};


};

#endif // VIDEO_PIXMAP_WIDGET_H
