//
// Created by hsyuan on 2019-02-28.
//

#ifndef FACE_DEMO_SYSTEM_BM_FACE_SDK_WRAPPER_H
#define FACE_DEMO_SYSTEM_BM_FACE_SDK_WRAPPER_H

#include <iostream>
#include <string>
#include <memory>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/time.h"
#include "libavutil/imgutils.h"
}

#include <opencv2/opencv.hpp>

namespace fdrtsp {

    struct FaceRect {
        float x1, y1, x2, y2;
    };

    struct FaceLandmarkPoints {
        float x[5];
        float y[5];
        float score;
    };

    typedef std::vector<FaceRect> FaceRectVector;
    typedef std::vector<float> FaceFeature;
    typedef std::vector<FaceFeature> FaceFeatureVector;

    class FaceDB;
    using FaceDBPtr = std::shared_ptr<FaceDB>;

    struct FaceRecognitionInfo {
        std::string label;
        float similar;
    };

    struct ImageFaceInfo {
        FaceRectVector vct_face_rect;
        FaceFeatureVector vct_face_feature;
        std::vector<FaceLandmarkPoints> vct_landmark_points;
        std::vector<FaceRecognitionInfo> vct_face_labels;
    };

    struct FaceSdkParameters {
        int dev_id;
        int net_mode;
        int net_prec;
        int store_mode;
        std::string bmcnn_path;
        std::string detect_caffe_model_file;
        std::string detect_caffe_proto_file;

        std::string extract_caffe_model_file;
        std::string extract_caffe_proto_file;

        std::string landmark_caffe_model_file;
        std::string landmark_caffe_proto_file;
        FaceDBPtr db_ptr;

        FaceSdkParameters() : dev_id(0), net_mode(0), net_prec(0), store_mode(0) {
            std::cout << "FaceSdkParameters() ctor" << std::endl;
        }
    };


    class FaceSdk {
    public:
        static std::shared_ptr<FaceSdk> create();
        static int get_device_num();

        virtual ~FaceSdk() {
            std::cout << "~FaceSdk() dtor" << std::endl;
        }

        virtual int init(const FaceSdkParameters &params) = 0;

        virtual int detect(std::vector<AVFrame *> frames, std::vector<FaceRectVector> &faces) = 0;

        virtual int detect(std::vector<cv::Mat> &images, std::vector<FaceRectVector> &faces) = 0;

        virtual int extract(std::vector<cv::Mat> &images, std::vector<ImageFaceInfo>& img_face_info) = 0;

        virtual int extract(std::vector<AVFrame*> &frames, std::vector<ImageFaceInfo>& img_face_info) = 0;

        virtual float similarity(FaceFeature &feature1, FaceFeature &feature2) = 0;

    };

    using FaceSdkPtr = std::shared_ptr<FaceSdk>;
}


#endif //FACE_DEMO_SYSTEM_BM_FACE_SDK_WRAPPER_H
