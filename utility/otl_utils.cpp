//
// Created by hsyuan on 2019-02-28.
//

#include "otl_utils.h"
#include <memory>

#ifdef _WIN32
#define DASH '\\'
#else
#define DASH '/'
#endif

static const char *get_file_name(const char *filepath){
    const char *p = strrchr(filepath, DASH);
    return ++p;
}


static int gflog_level = LOG_ERROR;
void flog_set_level(int level) {
    gflog_level = level;
}

void flog_printf(int level, const char *file, int line, const char *format, ...)
{
    if (level > gflog_level) return;

    char msg_buf[1024];
    time_t tnow;
    time(&tnow);

    va_list args;
    va_start(args, format);
    vsnprintf(msg_buf, sizeof(msg_buf), format, args);
    va_end(args);


    auto tm = localtime(&tnow);
    if (level > 0) {
        if (level == LOG_ERROR) {
            printf("\033[31m%.2d:%.2d:%.2d %s,%d %s\n\033[0m", tm->tm_hour, tm->tm_min, tm->tm_sec, get_file_name(file),
                   line, msg_buf);
        }else{
            printf("%.2d:%.2d:%.2d %s,%d %s\n", tm->tm_hour, tm->tm_min, tm->tm_sec, get_file_name(file), line, msg_buf);
        }
    }
}


#if USE_OPENCV
int avframe_to_cvmat(const AVFrame *src, cv::Mat &out_img)
{
    uint8_t /**src_data[4],*/ *dst_data[4];
    int /*src_linesize[4],*/ dst_linesize[4];
    int src_w = 320, src_h = 240, dst_w, dst_h;

    struct SwsContext *convert_ctx = NULL;
    enum AVPixelFormat src_pix_fmt = (enum AVPixelFormat)src->format;
    enum AVPixelFormat dst_pix_fmt = AV_PIX_FMT_BGR24;


    src_w = src->width;
    dst_w = src->width;
    src_h = src->height;
    dst_h = src->height;

    out_img = cv::Mat(dst_h, dst_w, CV_8UC3);

    av_image_fill_arrays(dst_data, dst_linesize, out_img.data, dst_pix_fmt, dst_w, dst_h, 1);

    convert_ctx = sws_getContext(src_w, src_h, src_pix_fmt, dst_w, dst_h, dst_pix_fmt,
        SWS_FAST_BILINEAR, NULL, NULL, NULL);

    sws_scale(convert_ctx, src->data, src->linesize, 0, dst_h,
        dst_data, dst_linesize);

    sws_freeContext(convert_ctx);

    return 0;
}

AVFrame* cvmat_to_avframe(const cv::Mat& src, int to_pix_fmt)
{
    uint8_t *src_data[4]/*, *dst_data[4]*/;
    int src_linesize[4]/*, dst_linesize[4]*/;
    int src_w = 320, src_h = 240, dst_w, dst_h;

    enum AVPixelFormat src_pix_fmt = AV_PIX_FMT_BGR24;
    enum AVPixelFormat dst_pix_fmt = (enum AVPixelFormat)to_pix_fmt;
    struct SwsContext *convert_ctx = NULL;

    src_w = dst_w = src.cols;
    src_h = dst_h = src.rows;

    convert_ctx = sws_getContext(src_w, src_h, src_pix_fmt, dst_w, dst_h, (enum AVPixelFormat)dst_pix_fmt,
        SWS_FAST_BILINEAR, NULL, NULL, NULL);
    if (nullptr == convert_ctx) {
        printf("sws_getContext failed!");
        return nullptr;
    }

    av_image_fill_arrays(src_data, src_linesize, src.data, src_pix_fmt, src_w, src_h, 1);

    AVFrame *dst = av_frame_alloc();
    av_image_alloc(dst->data, dst->linesize, dst_w, dst_h, (enum AVPixelFormat) dst_pix_fmt, 1);

    dst->format = dst_pix_fmt;
    dst->width = dst_w;
    dst->height = dst_h;
    //
    int ret = sws_scale(convert_ctx, src_data, src_linesize, 0, dst_h,
        dst->data, dst->linesize);
    if (ret < 0) {
        printf("sws_scale err\n");
    }

    sws_freeContext(convert_ctx);

    return dst;
}

#endif //!USE_OPENCV

int bm_copy_file(const char* src, const char *dst)
{
    char buffer[1024];
    FILE *in, *out;
    if ((in = fopen(src, "r")) == NULL) {
        flog(LOG_ERROR, "open srcfile=%s failed\n", src);
        return -1;
    }

    if ((out = fopen(dst, "w")) == NULL) {
        flog(LOG_ERROR, "open dst file=%s failed, %s\n", dst, strerror(errno));
        fclose(in);
        return -1;
    }

    int len;
    while ((len = fread(buffer, 1, 1024, in)) > 0) {
        fwrite(buffer, 1, len, out);
    }

    fclose(out);
    fclose(in);
    return 0;
}

bool bm_is_image_ok(const char* src)
{
    FILE *fp = fopen(src, "rb");
    if (fp == nullptr) {
        return false;
    }

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<uint8_t[]> buffer(new uint8_t[length]);
    size_t n = fread(buffer.get(), 1, length, fp);
    fclose(fp);

    //
    // PNG check
    // PNG Header: 137 80 78 71 13 10 26 10
    // PNG IEND：00 00 00 00 0x49 0x45 0x4E 0x44 0xAE 0x42 0x60 0x82

    if (buffer[0] == 137 && buffer[1] == 80 && buffer[2] == 78 && buffer[3] == 71 && buffer[4] == 13
        && buffer[5] == 10 && buffer[6] == 26 && buffer[7] == 10)
    {
        if (buffer[length - 5] == 68 && buffer[length - 4] == 174 && buffer[length - 3] == 66
            && buffer[length - 2] == 96 && buffer[length - 1] == 130){
            return true;
        }

        for (int i = length - 1; i > length / 2; --i)
        {
            if (buffer[i - 5] == 68 && buffer[i - 4] == 174 && buffer[i - 3] == 66
                && buffer[i - 2] == 96 && buffer[i - 1] == 130)
                return true;
        }


    }
    else if (buffer[0] == 66 && buffer[1] == 77)//bmp
    {//BMP:check length of file

        char str[255];
        snprintf(str, 255, "0x%x%x%x%x", buffer[5], buffer[4], buffer[3], buffer[2]);

        int iLength = std::stoi(str, nullptr, 16);
        if (iLength <= length)
            return true;
    }
    else if (buffer[0] == 71 && buffer[1] == 73 && buffer[2] == 70 && buffer[3] == 56)
    {// GIF: Check 00 and 0x3B

        if (buffer[length - 2] == 0 && buffer[length - 1] == 59)
            return true;

        for (int i = length - 1; i > length / 2; --i)
        {
            if (buffer[i] != 0)
            {
                if (buffer[i] == 59 && buffer[i - 1] == 0)
                    return true;
            }
        }
    }
    else if (buffer[0] == 255 && buffer[1] == 216) //jpg
    {

        if (buffer[length - 2] == 0xff && buffer[length - 1] == 0xd9) {
            return true;
        }
        else
        {
            for (int i = length - 2; i > length / 2; --i)
            {
                if (buffer[i] == 0xff && buffer[i + 1] == 0xd9)
                    return true;
            }
        }
    }

    return false;
}

AVFrame * bm_avframe_dup(const AVFrame *src){

    AVFrame *copyFrame = av_frame_alloc();
    copyFrame->format = src->format;
    copyFrame->width = src->width;
    copyFrame->height = src->height;
    copyFrame->channels = src->channels;
    copyFrame->channel_layout = src->channel_layout;
    copyFrame->nb_samples = src->nb_samples;
    int ret = av_frame_get_buffer(copyFrame, 4);
    if (ret < 0) {
        goto Failed;
    }

    ret = av_frame_copy(copyFrame, src);
    if (ret < 0) {
        goto Failed;
    }
    ret = av_frame_copy_props(copyFrame, src);
    if (ret < 0) {
        goto Failed;
    }
    return copyFrame;
    Failed:
    av_frame_free(&copyFrame);
    return nullptr;
}



#define min(X,Y) ((X) < (Y) ? (X) : (Y))

#define UUID_SIZE 16

//FFMPEG uuid
//static unsigned char uuid[] = { 0xdc, 0x45, 0xe9, 0xbd, 0xe6, 0xd9, 0x48, 0xb7, 0x96, 0x2c, 0xd8, 0x20, 0xd9, 0x23, 0xee, 0xef };

//self UUID
static unsigned char uuid[] = { 0x54, 0x80, 0x83, 0x97, 0xf0, 0x23, 0x47, 0x4b, 0xb7, 0xf7, 0x4f, 0x32, 0xb5, 0x4e, 0x06, 0xac };

//��ʼ��
static unsigned char start_code[] = { 0x00,0x00,0x00,0x01 };


uint32_t h264sei_calc_nalu_size(uint32_t content)
{
    //SEI payload size
    uint32_t sei_payload_size = content + UUID_SIZE;
    //NALU + payload���� + ���ݳ��� + ����
    uint32_t sei_size = 1 + 1 + (sei_payload_size / 0xFF + (sei_payload_size % 0xFF != 0 ? 1 : 0)) + sei_payload_size;
    //��ֹ��
    uint32_t tail_size = 2;
    if (sei_size % 2 == 1)
    {
        tail_size -= 1;
    }
    sei_size += tail_size;

    return sei_size;
}

uint32_t h264sei_calc_packet_size(uint32_t sei_size)
{
    // Packet size = NALUSize + StartCodeSize
    return h264sei_calc_nalu_size(sei_size) + 4;
}

int h264sei_packet_write(unsigned char * packet, bool isAnnexb, const uint8_t * content, uint32_t size)
{
    unsigned char * data = (unsigned char*)packet;
    //unsigned int nalu_size = (unsigned int)h264sei_calc_nalu_size(size);
    //uint32_t sei_size = nalu_size;


    memcpy(data, start_code, sizeof(start_code));

    data += sizeof(unsigned int);

    //unsigned char * sei = data;
    //NAL header
    *data++ = 6; //SEI
    //sei payload type
    *data++ = 5; //unregister
    size_t sei_payload_size = size + UUID_SIZE;

    while (true)
    {
        *data++ = (sei_payload_size >= 0xFF ? 0xFF : (char)sei_payload_size);
        if (sei_payload_size < 0xFF) break;
        sei_payload_size -= 0xFF;
    }

    //UUID
    memcpy(data, uuid, UUID_SIZE);
    data += UUID_SIZE;

    memcpy(data, content, size);
    data += size;

    *data = 0x80;
    data++;


    return data - packet;
}

static int get_sei_buffer(unsigned char * data, uint32_t size, uint8_t * buff, int buf_size)
{
    unsigned char * sei = data;
    int sei_type = 0;
    unsigned sei_size = 0;
    //payload type
    do {
        sei_type += *sei;
    } while (*sei++ == 255);

    do {
        sei_size += *sei;
    } while (*sei++ == 255);

    if (sei_size >= UUID_SIZE && sei_size <= (data + size - sei) &&
        sei_type == 5 && memcmp(sei, uuid, UUID_SIZE) == 0)
    {
        sei += UUID_SIZE;
        sei_size -= UUID_SIZE;

        if (buff != NULL && buf_size != 0)
        {
            if (buf_size > (int)sei_size)
            {
                memcpy(buff, sei, sei_size);
            }else{
                printf("ERROR:input buffer(%d) < SEI size(%d)\n", buf_size, sei_size);
                return -1;
            }
        }

        return sei_size;
    }
    return -1;
}

int h264sei_packet_read(unsigned char * packet, uint32_t size, uint8_t * buffer, int buf_size)
{
    unsigned char ANNEXB_CODE_LOW[] = { 0x00,0x00,0x01 };
    unsigned char ANNEXB_CODE[] = { 0x00,0x00,0x00,0x01 };

    unsigned char *data = packet;
    bool isAnnexb = false;
    if ((size > 3 && memcmp(data, ANNEXB_CODE_LOW, 3) == 0) ||
        (size > 4 && memcmp(data, ANNEXB_CODE, 4) == 0)
            )
    {
        isAnnexb = true;
    }

    if (isAnnexb)
    {
        while (data < packet + size) {
            if ((packet + size - data) > 4 && data[0] == 0x00 && data[1] == 0x00)
            {
                int startCodeSize = 2;
                if (data[2] == 0x01)
                {
                    startCodeSize = 3;
                }
                else if (data[2] == 0x00 && data[3] == 0x01)
                {
                    startCodeSize = 4;
                }

                if (startCodeSize == 3 || startCodeSize == 4)
                {
                    if ((packet + size - data) > (startCodeSize + 1))
                    {
                        //SEI
                        unsigned char * sei = data + startCodeSize + 1;

                        int ret = get_sei_buffer(sei, (packet + size - sei), buffer, buf_size);
                        if (ret != -1)
                        {
                            return ret;
                        }
                    }
                    data += startCodeSize + 1;
                }
                else
                {
                    data += startCodeSize + 1;
                }
            }
            else
            {
                data++;
            }
        }
    }
    else
    {
        printf("can't find NALU startCode\n");
    }
    return -1;
}


int h265sei_packet_write(unsigned char * packet, bool isAnnexb, const uint8_t * content, uint32_t size)
{
    unsigned char * data = (unsigned char*)packet;
    //unsigned int nalu_size = (unsigned int)h264sei_calc_nalu_size(size);
    //uint32_t sei_size = nalu_size;

    memcpy(data, start_code, sizeof(start_code));

    data += sizeof(unsigned int);

    uint8_t nalUnitType = 39;
    //unsigned char * sei = data;
    //NAL header
    *data++ = (uint8_t)nalUnitType << 1;
    *data++ = 1 + (nalUnitType == 2);
    //sei payload type
    *data++ = 5; //unregister
    size_t sei_payload_size = size + UUID_SIZE;

    while (true)
    {
        *data++ = (sei_payload_size >= 0xFF ? 0xFF : (char)sei_payload_size);
        if (sei_payload_size < 0xFF) break;
        sei_payload_size -= 0xFF;
    }

    //UUID
    memcpy(data, uuid, UUID_SIZE);
    data += UUID_SIZE;

    memcpy(data, content, size);
    data += size;

    *data = 0x80;
    data++;


    return data - packet;
}

int h265sei_packet_read(unsigned char * packet, uint32_t size, uint8_t * buffer, int buf_size)
{
    unsigned char ANNEXB_CODE_LOW[] = { 0x00,0x00,0x01 };
    unsigned char ANNEXB_CODE[] = { 0x00,0x00,0x00,0x01 };

    unsigned char *data = packet;
    bool isAnnexb = false;
    if ((size > 3 && memcmp(data, ANNEXB_CODE_LOW, 3) == 0) ||
        (size > 4 && memcmp(data, ANNEXB_CODE, 4) == 0)
            )
    {
        isAnnexb = true;
    }

    if (isAnnexb)
    {
        while (data < packet + size) {
            if ((packet + size - data) > 4 && data[0] == 0x00 && data[1] == 0x00)
            {
                int startCodeSize = 2;
                if (data[2] == 0x01)
                {
                    startCodeSize = 3;
                }
                else if (data[2] == 0x00 && data[3] == 0x01)
                {
                    startCodeSize = 4;
                }

                if (startCodeSize == 3 || startCodeSize == 4)
                {
                    if ((packet + size - data) > (startCodeSize + 2))
                    {
                        //SEI
                        unsigned char * sei = data + startCodeSize + 2;

                        int ret = get_sei_buffer(sei, (packet + size - sei), buffer, buf_size);
                        if (ret != -1)
                        {
                            return ret;
                        }
                    }
                    data += startCodeSize + 2;
                }
                else
                {
                    data += startCodeSize + 2;
                }
            }
            else
            {
                data++;
            }
        }
    }
    else
    {
        printf("can't find NALU startCode\n");
    }
    return -1;
}
