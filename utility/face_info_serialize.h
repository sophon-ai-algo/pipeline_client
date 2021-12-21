//
// Created by hsyuan on 2019-03-20.
//

#ifndef FACEDEMOEXAMPLE_FACE_INFO_SERIALIZE_H
#define FACEDEMOEXAMPLE_FACE_INFO_SERIALIZE_H

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#ifndef _WIN32
#include <netinet/in.h>
#else
#include <WinSock2.h>
#include <Windows.h>
#endif
#include <memory>

#ifndef BMCV_API_EXT_H
typedef struct bmcv_rect_struct {
    int start_x, start_y, crop_w, crop_h;
}bmcv_rect_t;
#endif

namespace bm {

    class NoCopyable {
    protected:
        NoCopyable() = default;

        ~NoCopyable() = default;

        NoCopyable(const NoCopyable &) = delete;

        NoCopyable &operator=(const NoCopyable &rhs) = delete;
    };
    class ByteBuffer : public NoCopyable {
        char *m_bytes;
        int m_back_offset;
        int m_front_offset;
        size_t m_size;

        bool check_buffer(int len) {
            if (m_back_offset + len > m_size) {
                m_bytes = (char*)realloc(m_bytes, m_size + 1024);
                assert(m_bytes != nullptr);
                if (m_bytes == nullptr) {
                    return false;
                }
            }

            return true;
        }

        bool check_buffer2(int len) {
            if (m_back_offset - len < 0) {
                return false;
            }

            return true;
        }

        int push_internal(int8_t *p, int len) {
            if (!check_buffer(len)) return -1;
            memcpy((uint8_t*)m_bytes + m_back_offset, p, len);
            m_back_offset += len;
            return 0;
        }

        int pop_internal(int8_t *p, int len) {
            if (check_buffer2(len) !=0) return -1;
            memcpy(p, &m_bytes[m_back_offset], len);
            m_back_offset-=len;
            return 0;
        }

        int pop_front_internal(int8_t *p, int len) {
            if (m_front_offset + len > m_back_offset) return -1;
            memcpy(p, &m_bytes[m_front_offset], len);
            m_front_offset+=len;
            return 0;
        }

        uint64_t bm_htonll(uint64_t val)
        {
            return (((uint64_t) htonl(val)) << 32) + htonl(val >> 32);
        }

        uint64_t bm_ntohll(uint64_t val)
        {
            return (((uint64_t) ntohl(val)) << 32) + ntohl(val >> 32);
        }

        std::function<void(void*)> m_freeFunc;

    public:
        ByteBuffer(size_t size = 1024):m_size(size) {
            m_bytes = new char[size];
            assert(m_bytes != nullptr);
            m_front_offset = 0;
            m_back_offset = 0;

            m_freeFunc = [this](void*p) {
                delete [] m_bytes;
            };

        }

        ByteBuffer(const char *buf, int size, std::function<void(void*)> free_func = nullptr){
            m_bytes = (char*)buf;
            m_front_offset = 0;
            m_back_offset = size;
            m_size = size;
            m_freeFunc = free_func;
        }

        ~ByteBuffer() {
            if (m_freeFunc) m_freeFunc(m_bytes);
        }



        int push_back(int8_t b){
            return push_internal(&b, sizeof(b));
        }

        int push_back(uint8_t b) {
            return push_internal((int8_t *)&b, sizeof(b));
        }

        int push_back(int16_t b) {
            b = htons(b);
            int8_t  *p = (int8_t*)&b;
            return push_internal(p, sizeof(b));
        }

        int push_back(uint16_t b)
        {
            b = htons(b);
            int8_t  *p = (int8_t*)&b;
            return push_internal(p, sizeof(b));
        }

        int push_back(int32_t b)
        {
            b = htonl(b);
            int8_t  *p = (int8_t*)&b;
            return push_internal(p, sizeof(b));
        }

        int push_back(uint32_t b)
        {
            b = htonl(b);
            int8_t  *p = (int8_t*)&b;
            return push_internal(p, sizeof(b));
        }

        int push_back(int64_t b)
        {
            b = bm_htonll(b);
            int8_t  *p = (int8_t*)&b;
            return push_internal(p, sizeof(b));
        }

        int push_back(uint64_t b)
        {
            b = bm_htonll(b);
            int8_t  *p = (int8_t*)&b;
            return push_internal(p, sizeof(b));
        }

        int push_back(float f)
        {
            int8_t  *p = (int8_t*)&f;
            return push_internal(p, sizeof(f));
        }

        int push_back(double d)
        {
            int8_t  *p = (int8_t*)&d;
            return push_internal(p, sizeof(d));
        }

        int pop(int8_t &val) {
            return pop_internal(&val, sizeof(val));
        }

        int pop(int16_t &val) {
            int16_t t;
            if (pop_internal((int8_t *)&t, sizeof(val)) != 0) {
                return -1;
            }

            val = ntohs(t);
            return 0;
        }

        int pop(int32_t &val) {
            int32_t t;
            if (pop_internal((int8_t *)&t, sizeof(val)) != 0) {
                return -1;
            }

            val = ntohl(t);
            return 0;
        }
        int pop(int64_t &val) {
            int64_t t;
            if (pop_internal((int8_t *)&t, sizeof(val)) != 0) {
                return -1;
            }

            val = bm_ntohll(t);
            return 0;
        }

        int pop(uint8_t &val) {
            return pop_internal((int8_t*)&val, sizeof(val));
        }

        int pop(uint16_t &val) {
            uint16_t t;
            if (pop_internal((int8_t *)&t, sizeof(val)) != 0) {
                return -1;
            }

            val = ntohs(t);
            return 0;
        }

        int pop(uint32_t &val) {
            uint32_t t;
            if (pop_internal((int8_t *)&t, sizeof(val)) != 0) {
                return -1;
            }

            val = ntohl(t);
            return 0;
        }

        int pop(uint64_t &val) {
            uint64_t t;
            if (pop_internal((int8_t *)&t, sizeof(val)) != 0) {
                return -1;
            }

            val = bm_ntohll(t);
            return 0;
        }

        int pop(float &val) {
            return pop_internal((int8_t *)&val, sizeof(val));
        }

        int pop(double &val) {
            return pop_internal((int8_t *)&val, sizeof(val));
        }

        int pop_front(int8_t &val) {
            return pop_front_internal(&val, sizeof(val));
        }

        int pop_front(int16_t &val) {
            int16_t t;
            if (pop_front_internal((int8_t *)&t, sizeof(val)) != 0) {
                return -1;
            }

            val = ntohs(t);
            return 0;
        }

        int pop_front(int32_t &val) {
            int32_t t;
            if (pop_front_internal((int8_t *)&t, sizeof(val)) != 0) {
                return -1;
            }

            val = ntohl(t);
            return 0;
        }
        int pop_front(int64_t &val) {
            int64_t t;
            if (pop_front_internal((int8_t *)&t, sizeof(val)) != 0) {
                return -1;
            }

            val = bm_ntohll(t);
            return 0;
        }

        int pop_front(uint8_t &val) {
            return pop_front_internal((int8_t*)&val, sizeof(val));
        }

        int pop_front(uint16_t &val) {
            uint16_t t;
            if (pop_front_internal((int8_t *)&t, sizeof(val)) != 0) {
                return -1;
            }

            val = ntohs(t);
            return 0;
        }

        int pop_front(uint32_t &val) {
            uint32_t t;
            if (pop_front_internal((int8_t *)&t, sizeof(val)) != 0) {
                return -1;
            }

            val = ntohl(t);
            return 0;
        }

        int pop_front(uint64_t &val) {
            uint64_t t;
            if (pop_front_internal((int8_t *)&t, sizeof(val)) != 0) {
                return -1;
            }

            val = bm_ntohll(t);
            return 0;
        }

        int pop_front(float &val) {
            return pop_front_internal((int8_t *)&val, sizeof(val));
        }

        int pop_front(double &val) {
            return pop_front_internal((int8_t *)&val, sizeof(val));
        }

        char *data() {
            return m_bytes;
        }

        int size() {
            return m_back_offset - m_front_offset;
        }

    };

    struct Data : public NoCopyable {
        size_t dsize;
        uint8_t *data;

        Data() : dsize(0), data(nullptr){
        }

        Data(uint8_t* p, size_t sz) : dsize(sz), data(p) {
        }

        virtual ~Data() {
            if (data)  delete[] data;
        }

        int size() {
            return dsize;
        }

        template <typename T>
        T* ptr() {
            return static_cast<T*>(data);
        }

    };

    using DataPtr = std::shared_ptr<Data>;

    struct NetOutputObject {
        float x1, y1, x2, y2;
        float score;
        int class_id;

        int width() {
            return x2-x1;
        }

        int height() {
            return y2-y1;
        }
    };
    using NetOutputObjects =std::vector<NetOutputObject>;

    struct PoseKeyPoints{
        std::vector<float> keypoints;
        std::vector<int> shape;
        int width, height;
    };

    struct SafetyhatObject {
        float x1, y1, x2, y2;
        float score;
        int class_id;
        int index;
        float confidence;

        int width() {
            return x2-x1;
        }

        int height() {
            return y2-y1;
        }

        void to_bmcv_rect(bmcv_rect_t *rect){
            rect->start_y =y1;
            rect->start_x = x1;
            rect->crop_w = width();
            rect->crop_h = height();
        }
       
    };
    using SafetyhatObjects = std::vector<SafetyhatObject>;

    struct NetOutputDatum {
        enum NetClassType {
            Box=0,
            Pose,
            FaceRecognition,
            SaftyhatRecogniton
        };
        NetClassType type;
        NetOutputObjects obj_rects;
        PoseKeyPoints pose_keypoints;
        SafetyhatObjects safetyhat_objects;
        NetOutputDatum(PoseKeyPoints& o) {
            pose_keypoints = o;
            type = Pose;
        }

        NetOutputDatum(NetOutputObjects& o) {
            type = Box;
            obj_rects = o;
        }

        NetOutputDatum() {
            type = Box;
        }

        NetOutputDatum(SafetyhatObjects& o) {
            safetyhat_objects = o;
            type = SaftyhatRecogniton;
        }
        std::shared_ptr<ByteBuffer> toByteBuffer() {
            std::shared_ptr<ByteBuffer> buf = std::make_shared<ByteBuffer>();
            buf->push_back((int32_t)type);
            if (Box == type) {
                buf->push_back((uint32_t)obj_rects.size());
                for(auto o: obj_rects) {
                    buf->push_back(o.x1);
                    buf->push_back(o.y1);
                    buf->push_back(o.x2);
                    buf->push_back(o.y2);
                    buf->push_back(o.score);
                    buf->push_back(o.class_id);
                }
            } else if (SaftyhatRecogniton == type) {
                buf->push_back((uint32_t)safetyhat_objects.size());
                for(auto o: safetyhat_objects) {
                    buf->push_back(o.x1);
                    buf->push_back(o.y1);
                    buf->push_back(o.x2);
                    buf->push_back(o.y2);
                    buf->push_back(o.score);
                    buf->push_back(o.class_id);
                    buf->push_back(o.index);
                    buf->push_back(o.confidence);
                }     
            } else if(Pose == type) {
                buf->push_back(pose_keypoints.height);
                buf->push_back(pose_keypoints.width);
                buf->push_back((uint32_t)pose_keypoints.shape.size());
                for(int i = 0;i < pose_keypoints.shape.size(); ++i) {
                    buf->push_back(pose_keypoints.shape[i]);
                }

                buf->push_back((uint32_t)pose_keypoints.keypoints.size());
                for(int i = 0;i < pose_keypoints.keypoints.size(); ++i) {
                    buf->push_back(pose_keypoints.keypoints[i]);
                }
            }else{
                printf("Unsupport type=%d\n", type);
                assert(0);
            }

            return buf;
        }

        void fromByteBuffer(ByteBuffer *buf) {
            int32_t itype;
            buf->pop_front(itype);
            this->type = (NetClassType)itype;
            if (Box == type) {
                uint32_t size = 0;
                buf->pop_front(size);
                for(int i = 0; i < size; ++i) {
                    NetOutputObject o;
                    buf->pop_front(o.x1);
                    buf->pop_front(o.y1);
                    buf->pop_front(o.x2);
                    buf->pop_front(o.y2);
                    buf->pop_front(o.score);
                    buf->pop_front(o.class_id);
                    obj_rects.push_back(o);
                }
            } else if(SaftyhatRecogniton == type) {
                uint32_t size = 0;
                buf->pop_front(size);
                for(int i = 0; i < size; ++i) {
                    SafetyhatObject o;
                    buf->pop_front(o.x1);
                    buf->pop_front(o.y1);
                    buf->pop_front(o.x2);
                    buf->pop_front(o.y2);
                    buf->pop_front(o.score);
                    buf->pop_front(o.class_id);
                    buf->pop_front(o.index);
                    buf->pop_front(o.confidence);
                    safetyhat_objects.push_back(o);
                }
            } else if(Pose == type) {
                buf->pop_front(pose_keypoints.height);
                buf->pop_front(pose_keypoints.width);
                uint32_t size=0;
                buf->pop_front(size);
                for(int i = 0;i < size; ++i) {
                    int dim = 0;
                    buf->pop_front(dim);
                    pose_keypoints.shape.push_back(dim);
                }

                buf->pop_front(size);
                pose_keypoints.keypoints.resize(size);
                for(int i = 0;i < size; ++i) {
                    buf->pop_front(pose_keypoints.keypoints[i]);
                }
            }else{
                printf("Unsupport type=%d\n", type);
                assert(0);
            }

        }
    };
}

#endif //FACEDEMOEXAMPLE_FACE_INFO_SERIALIZE_H
