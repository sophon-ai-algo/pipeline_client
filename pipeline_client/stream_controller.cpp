#include "stream_controller.h"
#include <qwidget.h>
#include "face_info_serialize.h"
#include "otl_string.h"

StreamController::StreamController(video_widget *pWidget, const std::string& strUrl, int channel):m_video_widget(pWidget),
m_rtsp_url(strUrl), m_rtsp_reader(channel)
{
    m_video_widget->SetTitle(QString("Channel-%1 | 1920x1080").arg(channel));
    m_rtsp_reader.set_observer(this);
}


StreamController::~StreamController()
{
}

int StreamController::get_state()
{
    return 0;
}

int StreamController::start_stream(const std::string& strmFmt, const std::string& pixelFmt, int w, int h)
{
    if (m_play_thread) {
        m_is_play_thread_running = false;
        m_play_thread->join();
        delete m_play_thread;
        m_play_thread = nullptr;
    }

    AVDictionary *opts=NULL;
    av_dict_set(&opts, "pcie_no_copyback", "0", 0);
    m_rtsp_reader.demuxer().set_param(strmFmt, pixelFmt, w, h);
    int ret = m_rtsp_reader.open_stream(m_rtsp_url, true, opts);
    if (ret < 0){
        return ret;
    }

    av_dict_free(&opts);

    m_is_play_thread_running = true;
    m_play_thread = new std::thread(&StreamController::video_play_thread_proc, this);
    return 0;
}

int StreamController::stop_stream()
{
    if (m_play_thread) {
        m_is_play_thread_running = false;
        m_play_thread->join();
        delete m_play_thread;
        m_play_thread = nullptr;
    }
    m_rtsp_reader.close_stream(false);
    return 0;
}

void StreamController::set_frame_bufferd_num(int num)
{
    //m_frame_buffered_num = num;
}

void StreamController::on_decoded_avframe(const AVPacket *pkt, const AVFrame *pFrame)
{
#if 0
    static int64_t last_frame_time = 0;
    if (last_frame_time == 0) {
        last_frame_time = av_gettime();
    }else{
        std::cout << "Frame Interval: " << (av_gettime() - last_frame_time)/1000 << std::endl;
        last_frame_time = av_gettime();
    }
#endif
    auto new_frame = av_frame_clone(pFrame);
    std::lock_guard<std::mutex> lck(m_framelist_sync);
    m_frameList.push_back(new_frame);
}




void StreamController::on_decoded_sei_info(const uint8_t *sei_data, int sei_data_len, uint64_t pkt_pts, int64_t pkt_pos)
{
    TestFaceInfo faceinfo;
    faceinfo.pkt_pts = pkt_pts;
    faceinfo.pkt_pos = pkt_pos;

    std::string sei_raw = String::base64_dec(sei_data, sei_data_len);
    bm::ByteBuffer buf(sei_raw.data(), sei_raw.size());
    faceinfo.datum.fromByteBuffer(&buf);
    std::lock_guard<std::mutex> lck(m_framelist_sync);
    m_faceList.push_back(faceinfo);

}

void StreamController::video_play_thread_proc() {

    while(m_is_play_thread_running){
        if (m_video_widget == nullptr || m_frameList.size() == 0){
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            continue;
        }

        std::lock_guard<std::mutex> lck(m_framelist_sync);
        if (m_frameList.size() > 0) {
            
            auto myFrame = m_frameList.front();
            auto cur_ts = av_gettime_relative();
            bool is_play = false;
            if (m_last_system_ts == 0) {
                is_play = true;
            }else {
                auto delta = myFrame->pkt_pts - m_last_pkt_pts;
                delta = delta > 40000 ? 40000:delta;
                if (m_last_system_ts + delta < cur_ts){
                    is_play = true;
                }
            }

            if (!is_play) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                continue;
            }


            int N = 1;

            if (m_frameList.size() > 2) N = 2;
            while(N-- > 0) {
                auto myFrame = m_frameList.front();

                //std::cout << "cache frames:" << m_frameList.size() << std::endl;
                //std::cout << "Frame Play Interval: " << (cur_ts - m_last_system_ts) / 1000 << std::endl;
                m_last_system_ts = cur_ts;
                m_last_pkt_pts = myFrame->pkt_pts;

                m_frameList.pop_front();
                auto render = m_video_widget->GetVideoHwnd();
                render->draw_frame(myFrame);
				av_usleep(28000);

                while (m_faceList.size() > 0) {
                    auto faceinfo = m_faceList.front();
                    if (AV_NOPTS_VALUE == faceinfo.pkt_pts) {
                        if (faceinfo.pkt_pos <= myFrame->pkt_pos) {
                            render->draw_info(faceinfo.datum);
                            m_faceList.pop_front();
                        } else {
                            break;
                        }
                    }else {
                        if (faceinfo.pkt_pts <= myFrame->pkt_pts) {
                            render->draw_info(faceinfo.datum);
                            m_faceList.pop_front();
                        } else {
                            break;
                        }
                    }
                }

                //Free frame
                av_frame_unref(myFrame);
                av_frame_free(&myFrame);
            }
        }

    }

}
