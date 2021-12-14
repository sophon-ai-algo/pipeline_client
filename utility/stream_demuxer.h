//
// Created by hsyuan on 2019-02-22.
//

#ifndef TESTUV_STREAM_DEMUXER_H
#define TESTUV_STREAM_DEMUXER_H

#include <iostream>
#include <thread>
#include <list>
#include "ffmpeg_global.h"

namespace fdrtsp {

    struct StreamDemuxerEvents {
        virtual void on_avformat_opened(AVFormatContext *ifmt_ctx) = 0;

        virtual void on_avformat_closed() = 0;

        virtual int on_read_frame(AVPacket *pkt) = 0;

        virtual void on_read_eof(AVPacket *pkt) = 0;
    };

    class StreamDemuxer : FfmpegGlobal {
        enum State : int8_t {
            Initialize = 0,
            Service,
            Down
        };

        AVFormatContext *m_ifmt_ctx;
        StreamDemuxerEvents *m_observer;
        State m_work_state;
        std::string m_inputUrl;
        std::thread *m_thread_reading;
        bool m_repeat;
        bool m_keep_running;

        std::string m_format_name;
        std::string m_pixel_fmt;
        int m_width, m_height;

    private:
        int do_initialize();

        int do_service();

        int do_down();

    public:
        StreamDemuxer();

        virtual ~StreamDemuxer();

        int open_stream(std::string url, StreamDemuxerEvents *observer, bool repeat = true, bool isSyncOpen=false);

        int close_stream(bool is_waiting);

        int set_param(const std::string& format_name, const std::string& pixel_format, int w, int h);

        //int get_codec_parameters(int stream_index, AVCodecParameters **p_codecpar);
        //int get_codec_type(int stream_index, int *p_codec_type);
    };
}



#endif //TESTUV_STREAM_DEMUXER_H
