#include <QPainter>
#include "video_pixmap_widget.h"
#include "ui_video_pixmap_widget.h"

video_pixmap_widget::video_pixmap_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::video_pixmap_widget)
{
    ui->setupUi(this);
    setUpdatesEnabled(true);
    m_refreshTimer = new QTimer(this);
    m_refreshTimer->setTimerType(Qt::PreciseTimer);
    connect(m_refreshTimer, SIGNAL(timeout()), this, SLOT(onRefreshTimeout()));
    m_refreshTimer->setInterval(25);
    m_refreshTimer->start();
}

video_pixmap_widget::~video_pixmap_widget()
{
    delete ui;
}

int video_pixmap_widget::frame_width()
{
    return m_avframe == NULL ? 0:m_avframe->width;
}

int video_pixmap_widget::frame_height()
{
    return m_avframe == NULL ? 0:m_avframe->height;
}

int video_pixmap_widget::draw_frame(const AVFrame *frame, bool bUpdate)
{
    std::lock_guard<std::mutex> lck(m_syncLock);
    if (m_avframe == nullptr) {
        m_avframe = av_frame_alloc();
    }else{
        av_frame_unref(m_avframe);
    }

    av_frame_ref(m_avframe, frame);

    if (bUpdate) {
        //QEvent *e = new QEvent(BM_UPDATE_VIDEO);
        //QCoreApplication::postEvent(this, e);
    }
    return 0;
}

int video_pixmap_widget::draw_rect(const fdrtsp::FaceRectVector& vct_rect, const std::vector<fdrtsp::FaceRecognitionInfo> &vct_label, bool bUpdate)
{
    std::lock_guard<std::mutex> lck(m_syncLock);
	if (vct_rect.size() > 0) {
        m_roi_heatbeat = 0;
        m_vct_face_rect = vct_rect;
        m_vct_label = vct_label;
        if (bUpdate) {
            //update();
        }
    }

    return 0;
}

void video_pixmap_widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    std::lock_guard<std::mutex> lck(m_syncLock);
    if (m_avframe != nullptr) {
        std::unique_ptr<uint8_t> ptr (avframe_to_rgb32(m_avframe));
        QImage origin = QImage(ptr.get(), m_avframe->width, m_avframe->height, QImage::Format_RGB32);
        if (m_vct_face_rect.size() > 0) {
            QPainter painter1(&origin);
            QPen redPen(Qt::red);
            redPen.setWidth(5);
            painter1.setPen(redPen);
            for(int i = 0; i < m_vct_face_rect.size(); ++i) {
                auto pt = m_vct_face_rect[i];
                QRect rc(pt.x1, pt.y1, pt.x2-pt.x1, pt.y2-pt.y1);
                painter1.drawRect(rc);
                if (m_vct_label.size() > 0) {
                    QString text = QString("%1-%2").arg(m_vct_label[i].label.c_str()).arg(m_vct_label[i].similar);
                    painter1.drawText(pt.x1-1, pt.y1-4, text);
                }
            }
            //m_vct_face_rect.clear();
        }

        QImage img = origin.scaled(geometry().size(), Qt::AspectRatioMode::IgnoreAspectRatio);
        painter.drawImage(0, 0, img);
    }

}

unsigned char* video_pixmap_widget::avframe_to_rgb32(const AVFrame *src)
{
    uint8_t /**src_data[4],*/ *dst_data[4];
    int /*src_linesize[4],*/ dst_linesize[4];
    int src_w = 320, src_h = 240, dst_w, dst_h;

    struct SwsContext *convert_ctx = NULL;
    enum AVPixelFormat src_pix_fmt = (enum AVPixelFormat)src->format;
    enum AVPixelFormat dst_pix_fmt = AV_PIX_FMT_RGB32;


    src_w = dst_w = src->width;
    src_h = dst_h = src->height;

    uint8_t *prgb32 = new uint8_t[src_w * src_h * 4];

    av_image_fill_arrays(dst_data, dst_linesize, prgb32, dst_pix_fmt, dst_w, dst_h, 16);
    convert_ctx = sws_getContext(src_w, src_h, src_pix_fmt, dst_w, dst_h, dst_pix_fmt,
                                 SWS_FAST_BILINEAR, NULL, NULL, NULL);

    sws_scale(convert_ctx, src->data, src->linesize, 0, dst_h,
              dst_data, dst_linesize);

    sws_freeContext(convert_ctx);

    return prgb32;
}

bool video_pixmap_widget::event(QEvent *e) {
    if (e->type() == BM_UPDATE_VIDEO) {
        repaint();
        return true;
    }

    return QWidget::event(e);
}

void video_pixmap_widget::onRefreshTimeout()
{
    repaint();
    m_roi_heatbeat++;
    if (m_roi_heatbeat > 8) {
        m_roi_heatbeat = 0;
        m_vct_face_rect.clear();
    }
}

int video_pixmap_widget::clear_frame() {
    std::lock_guard<std::mutex> lck(m_syncLock);
    if (m_avframe) {
        av_frame_free(&m_avframe);
    }
    return 0;
}