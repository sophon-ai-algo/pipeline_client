//
// Created by hsyuan on 2019-02-22.
//

#include "stream_demuxer.h"
#include "otl_utils.h"

namespace fdrtsp {

    StreamDemuxer::StreamDemuxer() : m_ifmt_ctx(nullptr), m_observer(nullptr),
                                     m_thread_reading(nullptr) {
        m_ifmt_ctx = avformat_alloc_context();
    }

    StreamDemuxer::~StreamDemuxer() {
        std::cout << "~StreamDemuxer() dtor..." << std::endl;
        close_stream(false);
        avformat_close_input(&m_ifmt_ctx);
        avformat_free_context(m_ifmt_ctx);
    }


/*int StreamDemuxer::get_codec_parameters(int stream_index, AVCodecParameters **p_codecpar)
{
    if (nullptr == m_ifmt_ctx || stream_index >= m_ifmt_ctx->nb_streams) {
        return -1;
    }

    *p_codecpar = m_ifmt_ctx->streams[stream_index]->codecpar;
    return 0;
}

int StreamDemuxer::get_codec_type(int stream_index, int *p_codec_type)
{
    if (nullptr == m_ifmt_ctx || stream_index >= m_ifmt_ctx->nb_streams) {
        return -1;
    }

    *p_codec_type = m_ifmt_ctx->streams[stream_index]->codecpar->codec_type;
    return 0;
}*/
    int StreamDemuxer::set_param(const std::string& format_name, const std::string& pixel_format, int w, int h)
    {
        m_format_name = format_name;
        m_pixel_fmt = pixel_format;
        m_width = w;
        m_height = h;
		return 0;
    }

    int StreamDemuxer::do_initialize() {

        std::string prefix = "rtsp://";
        AVDictionary *opts = NULL;
        AVInputFormat *fmt = nullptr;
        if (m_inputUrl.compare(0, prefix.size(), prefix) == 0) {
            av_dict_set(&opts, "rtsp_transport", "tcp", 0);
            //av_dict_set(&opts, "stimeout", "2000000", 0);
            //av_dict_set(&opts, "probesize", "400", 0);
            //av_dict_set(&opts, "analyzeduration", "100", 0);
            //av_dict_set(&opts, "rw_timeout", "15000", 0);
            //av_dict_set(&opts, "fflags", "igndts", 0);
        }else {
            fmt = av_find_input_format(m_format_name.c_str());
            if (m_format_name == "rawvideo") {
                av_dict_set(&opts, "pixel_format", m_pixel_fmt.c_str(), 0);
                av_dict_set_int(&opts, "width", m_width, 0);
                av_dict_set_int(&opts, "height", m_height, 0);
            }
        }


        std::cout << "Open stream " << m_inputUrl << std::endl;
       //av_find_input_format("mjpeg");
        int ret = avformat_open_input(&m_ifmt_ctx, m_inputUrl.c_str(), fmt, &opts);
        av_dict_free(&opts);
        if (ret < 0) {
            std::cout << "Can't open file " << m_inputUrl << std::endl;
            return ret;
        }

        ret = avformat_find_stream_info(m_ifmt_ctx, NULL);
        if (ret < 0) {
            std::cout << "Unable to get stream info" << std::endl;
            return ret;
        }

        std::cout << "Init:total stream num:" << m_ifmt_ctx->nb_streams << std::endl;
        if (m_observer) {
            m_observer->on_avformat_opened(m_ifmt_ctx);
        }

        // Enter Working service
        m_work_state = Service;

        return 0;
    }

    int StreamDemuxer::do_down() {
        // Close avformat_input
        avformat_close_input(&m_ifmt_ctx);

        if (m_observer) {
            m_observer->on_avformat_closed();
        }

        if (m_repeat) {
            m_work_state = Initialize;
        } else {
            m_keep_running = false;
        }

        return 0;
    }

    int StreamDemuxer::do_service() {
#if LIBAVCODEC_VERSION_MAJOR > 56
        AVPacket *pkt = av_packet_alloc();
#else
        AVPacket *pkt = (AVPacket*)av_malloc(sizeof(AVPacket));
        av_init_packet(pkt);
#endif
        while (Service == m_work_state) {
            int ret = av_read_frame(m_ifmt_ctx, pkt);
            if (ret < 0) {
                if (ret != AVERROR_EOF) continue;
                m_observer->on_read_eof(pkt);
                m_work_state = Down;
                break;
            }

            if (m_observer) {
                m_observer->on_read_frame(pkt);
            }

            av_packet_unref(pkt);
        }

#if LIBAVCODEC_VERSION_MAJOR > 56
        av_packet_free(&pkt);
#else
        av_free_packet(pkt);
        av_freep(&pkt);
#endif

        return 0;
    }

    int StreamDemuxer::open_stream(std::string url, StreamDemuxerEvents *observer,
            bool repeat, bool is_sync_open) {

        //First stop previous
        close_stream(false);

        m_inputUrl = url;
        m_observer = observer;
        m_repeat = repeat;
        m_work_state = Initialize;
        if (is_sync_open) {
            int ret = do_initialize();
            if (ret < 0) {
                return ret;
            }
        }

        m_keep_running = true;
        m_thread_reading = new std::thread([&] {
            while (m_keep_running) {
                switch (m_work_state) {
                    case Initialize:
                        if (do_initialize() != 0) {
                           std::this_thread::sleep_for(std::chrono::seconds(1));
                        }
                        break;
                    case Service:
                        do_service();
                        break;
                    case Down:
                        do_down();
                        break;
                }
            }
        });

        return 0;
    }

    int StreamDemuxer::close_stream(bool is_waiting) {
        if (!is_waiting) {
            m_work_state = Down;
            m_repeat = false;
        }

        if (nullptr != m_thread_reading) {
            m_thread_reading->join();
            delete m_thread_reading;
            m_thread_reading = nullptr;
        }

        return 0;
    }

}
