//
// Created by hsyuan on 2019-02-27.
//

#ifndef BM_FFMPEG_DECODE_TEST_STREAM_DECODE_H
#define BM_FFMPEG_DECODE_TEST_STREAM_DECODE_H

#include "stream_demuxer.h"

namespace fdrtsp {

    struct StreamDecoderEvents {
        virtual ~StreamDecoderEvents() {}

        virtual void on_decoded_avframe(const AVPacket *pkt, const AVFrame *pFrame) = 0;

        virtual void on_decoded_sei_info(const uint8_t *sei_data, int sei_data_len, uint64_t pts, int64_t pkt_pos){};
        virtual void on_stream_eof() {};
    };


    class StreamDecoder : public StreamDemuxerEvents {
        StreamDecoderEvents *m_observer;
    protected:
        std::list<AVPacket *> m_list_packets;
        AVCodecContext *m_dec_ctx{nullptr};
        AVCodecContext *m_external_dec_ctx {nullptr};
        int m_video_stream_index{0};
        int m_frame_decoded_num{0};
        StreamDemuxer m_demuxer;
        AVDictionary *m_opts_decoder{nullptr};
        bool m_is_waiting_iframe{true};
        int m_id{0};
        //Functions
        int create_video_decoder(AVFormatContext *ifmt_ctx);

        int put_packet(AVPacket *pkt);

        AVPacket *get_packet();

        void clear_packets();

        int decode_frame(AVPacket *pkt, AVFrame *pFrame);

        int get_video_stream_index(AVFormatContext *ifmt_ctx);
        bool is_key_frame(AVPacket *pkt);
        //
        //Overload StreamDemuxerEvents Interface.
        //
        virtual void on_avformat_opened(AVFormatContext *ifmt_ctx) override;

        virtual void on_avformat_closed() override;

        virtual int on_read_frame(AVPacket *pkt) override;

        virtual void on_read_eof(AVPacket *pkt) override;

    public:
        StreamDecoder(int id, AVCodecContext *decoder=nullptr);
        virtual ~StreamDecoder();

        int set_observer(StreamDecoderEvents *observer);

        int open_stream(std::string url, bool repeat = true, AVDictionary *opts=nullptr);

        int close_stream(bool is_waiting = true);

        StreamDemuxer& demuxer() {
            return m_demuxer;
        }

        //External utilities
        static AVPacket* ffmpeg_packet_alloc();
        static AVCodecContext* ffmpeg_create_decoder(enum AVCodecID id, AVDictionary **opts=nullptr);
    };
}


#endif //BM_FFMPEG_DECODE_TEST_STREAM_DECODE_H
