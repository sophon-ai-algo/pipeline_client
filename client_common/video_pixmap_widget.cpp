#include "video_pixmap_widget.h"
#include <QPainter>
#include "ui_video_pixmap_widget.h"

template <typename T>
inline int intRound(const T a) {
  return int(a + 0.5f);
}

template <typename T>
inline T fastMax(const T a, const T b) {
  return (a > b ? a : b);
}
video_pixmap_widget::video_pixmap_widget(QWidget* parent)
    : QWidget(parent), ui(new Ui::video_pixmap_widget) {
  ui->setupUi(this);
  setUpdatesEnabled(true);
  m_refreshTimer = new QTimer(this);
  m_refreshTimer->setTimerType(Qt::PreciseTimer);
  connect(m_refreshTimer, SIGNAL(timeout()), this, SLOT(onRefreshTimeout()));
  m_refreshTimer->setInterval(25);
  m_refreshTimer->start();
}

video_pixmap_widget::~video_pixmap_widget() {
  delete ui;
}

int video_pixmap_widget::frame_width() {
  return m_avframe == NULL ? 0 : m_avframe->width;
}

int video_pixmap_widget::frame_height() {
  return m_avframe == NULL ? 0 : m_avframe->height;
}

int video_pixmap_widget::draw_frame(const AVFrame* frame) {
  std::lock_guard<std::mutex> lck(m_syncLock);
  if (m_avframe == nullptr) {
    m_avframe = av_frame_alloc();
  } else {
    av_frame_unref(m_avframe);
  }

  av_frame_ref(m_avframe, frame);
  return 0;
}

int video_pixmap_widget::draw_info(const bm::NetOutputDatum& datum) {
  std::lock_guard<std::mutex> lck(m_syncLock);
  m_info = datum;

  return 0;
}

void video_pixmap_widget::paintEvent(QPaintEvent* event) {
  QPainter painter(this);
  std::lock_guard<std::mutex> lck(m_syncLock);
  if (m_avframe != nullptr) {
    std::unique_ptr<uint8_t> ptr(avframe_to_rgb32(m_avframe));
    QImage origin = QImage(ptr.get(), m_avframe->width, m_avframe->height,
                           QImage::Format_RGB32);
    if (m_info.type == bm::NetOutputDatum::Box)
      drawBox(origin);
    if (m_info.type == bm::NetOutputDatum::Pose)
      drawPose(origin);
    if (m_info.type == bm::NetOutputDatum::SaftyhatRecogniton)
      drawSafetyHat(origin);

    QImage img = origin.scaled(geometry().size(),
                               Qt::AspectRatioMode::IgnoreAspectRatio);
    painter.drawImage(0, 0, img);
    //std::cout << "Draw image w=" << img.width() << std::endl;
  }
}

unsigned char* video_pixmap_widget::avframe_to_rgb32(const AVFrame* src) {
  uint8_t /**src_data[4],*/* dst_data[4];
  int /*src_linesize[4],*/ dst_linesize[4];
  int src_w = 320, src_h = 240, dst_w, dst_h;

  struct SwsContext* convert_ctx = NULL;
  enum AVPixelFormat src_pix_fmt = (enum AVPixelFormat)src->format;
  if (src_pix_fmt == AV_PIX_FMT_YUVJ420P) {
    src_pix_fmt = AV_PIX_FMT_YUV420P;
  }else if (src_pix_fmt == AV_PIX_FMT_YUVJ440P) {
      src_pix_fmt = AV_PIX_FMT_YUV440P;
  }

  enum AVPixelFormat dst_pix_fmt = AV_PIX_FMT_RGB32;

  src_w = dst_w = src->width;
  src_h = dst_h = src->height;

  uint8_t* prgb32 = new uint8_t[src_w * src_h * 4];

  av_image_fill_arrays(dst_data, dst_linesize, prgb32, dst_pix_fmt, dst_w,
                       dst_h, 16);
  convert_ctx =
      sws_getContext(src_w, src_h, src_pix_fmt, dst_w, dst_h, dst_pix_fmt,
                     SWS_FAST_BILINEAR, NULL, NULL, NULL);

  sws_scale(convert_ctx, src->data, src->linesize, 0, dst_h, dst_data,
            dst_linesize);

  sws_freeContext(convert_ctx);

  return prgb32;
}

bool video_pixmap_widget::event(QEvent* e) {
  if (e->type() == BM_UPDATE_VIDEO) {
    repaint();
    return true;
  }

  return QWidget::event(e);
}

void video_pixmap_widget::onRefreshTimeout() {
  repaint();
  m_roi_heatbeat++;
  if (m_roi_heatbeat > 8) {
    m_roi_heatbeat = 0;
  }
}

int video_pixmap_widget::clear_frame() {
  std::lock_guard<std::mutex> lck(m_syncLock);
  if (m_avframe) {
    av_frame_free(&m_avframe);
  }
  return 0;
}

void video_pixmap_widget::drawBox(QImage& dst) {
  Q_ASSERT(m_info.type == bm::NetOutputDatum::Box);
  if (m_info.obj_rects.size() > 0) {
    QPainter painter1(&dst);
    QPen redPen(Qt::green);
    redPen.setWidth(5);
    painter1.setPen(redPen);
    for (int i = 0; i < m_info.obj_rects.size(); ++i) {
        bm::NetOutputObject pt;
        if (m_info.track_rects.size() > i) {
            pt = m_info.track_rects[i];
        }else {
            pt = m_info.obj_rects[i];
        }

        QRect rc(pt.x1, pt.y1, pt.x2-pt.x1, pt.y2-pt.y1);
        painter1.drawRect(rc);

        QFont font("Arail", 40);
        painter1.setFont(font);
        QString text = QString("%1-%2").arg(pt.class_id).arg(pt.track_id);
        painter1.drawText(pt.x1-1, pt.y1-4, text);
    }
  }
}

void video_pixmap_widget::drawSafetyHat(QImage& dst) {
  Q_ASSERT(m_info.type == bm::NetOutputDatum::SaftyhatRecogniton);
  if (m_info.safetyhat_objects.size() > 0) {
    QPainter painter1(&dst);
    QPen redPen(Qt::green);
    redPen.setWidth(5);
    painter1.setPen(redPen);

    QFont font1("宋体", 20, QFont::Bold, true);

    for (int i = 0; i < m_info.safetyhat_objects.size(); ++i) {
      auto pt = m_info.safetyhat_objects[i];
      QRect rc(pt.x1, pt.y1, pt.x2 - pt.x1, pt.y2 - pt.y1);
      painter1.setPen(Qt::green);
      painter1.drawRect(rc);
      // redPen.setWidth(5);
      painter1.setFont(font1);
      painter1.setPen(Qt::red);
      QString text = QString("%1").arg(
          QString::fromLocal8Bit(m_vct_label[pt.index].c_str()));
      painter1.drawText(pt.x1 + 6, pt.y1 + 25, text);
      // if (m_vct_label.size() > 0) {
      //    QString text =
      //    QString("%1-%2").arg(m_vct_label[i].label.c_str()).arg(m_vct_label[i].similar);
      //    painter1.drawText(pt.x1-1, pt.y1-4, text);
      //}
    }
  }
}

void video_pixmap_widget::drawPose(QImage& dst) {
#define POSE_COCO_COLORS_RENDER_GPU                                            \
  255.f, 0.f, 85.f, 255.f, 0.f, 0.f, 255.f, 85.f, 0.f, 255.f, 170.f, 0.f,      \
      255.f, 255.f, 0.f, 170.f, 255.f, 0.f, 85.f, 255.f, 0.f, 0.f, 255.f, 0.f, \
      0.f, 255.f, 85.f, 0.f, 255.f, 170.f, 0.f, 255.f, 255.f, 0.f, 170.f,      \
      255.f, 0.f, 85.f, 255.f, 0.f, 0.f, 255.f, 255.f, 0.f, 170.f, 170.f, 0.f, \
      255.f, 255.f, 0.f, 255.f, 85.f, 0.f, 255.f
  Q_ASSERT(m_info.type == bm::NetOutputDatum::Pose);
  if (m_info.pose_keypoints.keypoints.size() == 0)
    return;
  const std::vector<float> POSE_COCO_COLORS_RENDER{POSE_COCO_COLORS_RENDER_GPU};
  const std::vector<unsigned int> POSE_COCO_PAIRS_RENDER{
      1,  2, 1,  5,  2,  3,  3,  4, 5, 6, 6,  7,  1,  8, 8,  9,  9,
      10, 1, 11, 11, 12, 12, 13, 1, 0, 0, 14, 14, 16, 0, 15, 15, 17};
  // Parameters
  const auto thicknessCircleRatio = 1.f / 75.f;
  const auto thicknessLineRatioWRTCircle = 0.75f;
  const auto& pairs = POSE_COCO_PAIRS_RENDER;
  const auto renderThreshold = 0.05;
  const auto scaleX = (float)dst.width() / m_info.pose_keypoints.width;
  const auto scaleY = (float)dst.height() / m_info.pose_keypoints.height;

  // Render keypoints
  renderKeyPointsCpu(
      dst, m_info.pose_keypoints.keypoints, m_info.pose_keypoints.shape, pairs,
      POSE_COCO_COLORS_RENDER, thicknessCircleRatio,
      thicknessLineRatioWRTCircle, renderThreshold, scaleX, scaleY);
}

void video_pixmap_widget::renderKeyPointsCpu(
    QImage& img,
    const std::vector<float>& keypoints,
    std::vector<int> keyshape,
    const std::vector<unsigned int>& pairs,
    const std::vector<float> colors,
    const float thicknessCircleRatio,
    const float thicknessLineRatioWRTCircle,
    const float threshold,
    float scaleX,
    float scaleY) {
  // Get frame channels
  const auto width = img.width();
  const auto height = img.height();
  const auto area = width * height;

  // Parameters
  const auto lineType = 8;
  const auto shift = 0;
  const auto numberColors = colors.size();
  const auto thresholdRectangle = 0.1f;
  const auto numberKeypoints = keyshape[1];
  QPainter painter(&img);
  // Keypoints
  for (auto person = 0; person < keyshape[0]; person++) {
    {
      const auto ratioAreas = 1;
      // Size-dependent variables
      const auto thicknessRatio = fastMax(
          intRound(std::sqrt(area) * thicknessCircleRatio * ratioAreas), 1);
      // Negative thickness in cv::circle means that a filled circle is to be
      // drawn.
      const auto thicknessCircle = (ratioAreas > 0.05 ? thicknessRatio : -1);
      const auto thicknessLine =
          2;  // intRound(thicknessRatio * thicknessLineRatioWRTCircle);
      const auto radius = thicknessRatio / 2;

      // Draw lines
      for (auto pair = 0u; pair < pairs.size(); pair += 2) {
        const auto index1 =
            (person * numberKeypoints + pairs[pair]) * keyshape[2];
        const auto index2 =
            (person * numberKeypoints + pairs[pair + 1]) * keyshape[2];
        if (keypoints[index1 + 2] > threshold &&
            keypoints[index2 + 2] > threshold) {
          const auto colorIndex =
              pairs[pair + 1] * 3;  // Before: colorIndex = pair/2*3;
          // const cv::Scalar color{ colors[(colorIndex+2) % numberColors],
          //                        colors[(colorIndex + 1) % numberColors],
          //                        colors[(colorIndex + 0) % numberColors] };
          // const cv::Point keypoint1{ intRound(keypoints[index1] * scale),
          // intRound(keypoints[index1 + 1] * scale) }; const cv::Point
          // keypoint2{ intRound(keypoints[index2] * scale),
          // intRound(keypoints[index2 + 1] * scale) }; cv::line(frame,
          // keypoint1, keypoint2, color, thicknessLine, lineType, shift);

          const QPoint keypoint1{intRound(keypoints[index1] * scaleX),
                                 intRound(keypoints[index1 + 1] * scaleY)};
          const QPoint keypoint2{intRound(keypoints[index2] * scaleX),
                                 intRound(keypoints[index2 + 1] * scaleY)};
          QPen pen;
          QColor color(colors[(colorIndex + 2) % numberColors],
                       colors[(colorIndex + 1) % numberColors],
                       colors[(colorIndex + 0) % numberColors]);
          pen.setColor(color);
          pen.setWidth(6);
          painter.setPen(pen);
          painter.drawLine(keypoint1, keypoint2);
        }
      }

      // Draw circles
      for (auto part = 0; part < numberKeypoints; part++) {
        const auto faceIndex = (person * numberKeypoints + part) * keyshape[2];
        if (keypoints[faceIndex + 2] > threshold) {
          const auto colorIndex = part * 3;
          // const cv::Scalar color{ colors[(colorIndex+2) % numberColors],
          //                        colors[(colorIndex + 1) % numberColors],
          //                        colors[(colorIndex + 0) % numberColors] };
          // const cv::Point center{ intRound(keypoints[faceIndex] * scale),
          // intRound(keypoints[faceIndex + 1] * scale) }; cv::circle(frame,
          // center, radius, color, thicknessCircle, lineType, shift);
          const QPoint center{intRound(keypoints[faceIndex] * scaleX),
                              intRound(keypoints[faceIndex + 1] * scaleY)};
          QPen pen;
          QColor color(colors[(colorIndex + 2) % numberColors],
                       colors[(colorIndex + 1) % numberColors],
                       colors[(colorIndex + 0) % numberColors]);
          pen.setColor(color);
          pen.setWidth(6);
          painter.setPen(pen);
          painter.drawEllipse(center, radius, radius);
        }
      }
    }
  }
}