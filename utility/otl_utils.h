//
// Created by hsyuan on 2019-02-28.
//

#ifndef PROJECT_BM_LOG_H
#define PROJECT_BM_LOG_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/time.h"
#include "libavutil/opt.h"
#include "libavutil/imgutils.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersrc.h"
#include "libavfilter/buffersink.h"
}

#if USE_OPENCV
#include <opencv2/opencv.hpp>
#include <chrono>

#endif //!USE_OPENCV



enum {
    LOG_QUITE = AV_LOG_QUIET,
    LOG_FATAL = AV_LOG_FATAL,
    LOG_ERROR = AV_LOG_ERROR,
    LOG_DEBUG = AV_LOG_DEBUG,
    LOG_WARNING = AV_LOG_WARNING,
    LOG_INFO = AV_LOG_INFO,
};


#define flog(level, fmt, ...) flog_printf(level, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
void flog_set_level(int level);
void flog_printf(int level, const char *file, int line, const char *format, ...);

#if USE_OPENCV
int avframe_to_cvmat(const AVFrame *src, cv::Mat &out_img);
AVFrame* cvmat_to_avframe(const cv::Mat& src, int to_pix_fmt);
#endif //!USE_OPENCV

int bm_copy_file(const char *src, const char *dst);
bool bm_is_image_ok(const char *src);

AVFrame *bm_avframe_dup(const AVFrame *src);

uint32_t reversebytes(uint32_t value);

uint32_t h264sei_calc_packet_size(uint32_t size);
int h264sei_packet_write(uint8_t *oPacketBuf, bool isAnnexb, const uint8_t *content, uint32_t size);
int h264sei_packet_read(uint8_t *inPacket, uint32_t size, uint8_t *buffer, int buff_size);

// H265
int h265sei_packet_write(unsigned char * packet, bool isAnnexb, const uint8_t * content, uint32_t size);
int h265sei_packet_read(unsigned char * packet, uint32_t size, uint8_t * buffer, int buf_size);

namespace fdrtsp {
    class otl_perf {
        int64_t start_us_;
        std::string tag_;
        int threshold_{50};
    public:
        otl_perf() {}

        ~otl_perf() {}

        void begin(const std::string &name, int threshold = 50) {
            tag_ = name;
            auto n = std::chrono::steady_clock::now();
            start_us_ = n.time_since_epoch().count();
            threshold_ = threshold;
        }

        void end() {
            auto n = std::chrono::steady_clock::now().time_since_epoch().count();
            auto delta = (n - start_us_) / 1000000;
            if (delta < threshold_) {
                flog(LOG_INFO, "%s used:%d", tag_.c_str(), delta);
            } else {
                flog(LOG_ERROR, "WARN:%s used:%d > %d", tag_.c_str(), delta, threshold_);
            }
        }
    };

    class RTPUtils {
    public:
        static int64_t rtp_time_to_ntp_us(int64_t delta, int freq = 90000) {
            return (double) delta / freq * 1000000;
        }
    };
}


#endif //PROJECT_BM_LOG_H
