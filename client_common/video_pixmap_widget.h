#ifndef VIDEO_PIXMAP_WIDGET_H
#define VIDEO_PIXMAP_WIDGET_H

#include <QWidget>
#include <QTimer>
#include <mutex>
#include "otl_utils.h"
#include "face_info_serialize.h"

namespace Ui {
class video_pixmap_widget;
}

#define BM_UPDATE_VIDEO (QEvent::Type)(QEvent::User + 1)

class IVideoDrawer {
public:
    virtual ~IVideoDrawer(){}
    virtual int frame_width() = 0;
    virtual int frame_height() = 0;
    virtual int draw_frame(const AVFrame *frame) = 0;
    virtual int draw_info(const bm::NetOutputDatum &info) = 0;
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
    virtual int draw_frame(const AVFrame *frame) override;
    virtual int draw_info(const bm::NetOutputDatum &info) override;
    int clear_frame() override;

protected:
    void paintEvent(QPaintEvent *event) override;
    bool event(QEvent *e) override;
protected slots:
    void onRefreshTimeout();
private:
    unsigned char* avframe_to_rgb32(const AVFrame *frame);
    void drawBox(QImage& dst);
    void drawPose(QImage& dst);
    void drawSafetyHat(QImage& dst);
    void renderKeyPointsCpu(QImage& img,const std::vector<float>& keypoints, std::vector<int> keyshape,
                            const std::vector<unsigned int>& pairs, const std::vector<float> colors,
                            const float thicknessCircleRatio, const float thicknessLineRatioWRTCircle,
                            const float threshold, float scaleX, float scaleY);
    Ui::video_pixmap_widget *ui;
    AVFrame *m_avframe {0};
    bm::NetOutputDatum m_info;
    QTimer *m_refreshTimer;
    std::mutex m_syncLock;
    int m_roi_heatbeat{0};

    std::string m_vct_label[4] = {
        /*  "helmet",
            "safety helmet",
            "hat",
            "no hat"*/
        "头盔",
        "安全帽",
        "普通帽子",
        "无帽子"
    };
};

#endif // VIDEO_PIXMAP_WIDGET_H
