//
// Created by hsyuan on 2019-03-20.
//

#ifndef FACEDEMOEXAMPLE_FACE_INFO_SERIALIZE_H
#define FACEDEMOEXAMPLE_FACE_INFO_SERIALIZE_H

#include "bm_face_sdk.h"
#include "otl_json.h"

class FaceInfoSerialize
{
public:
    ~FaceInfoSerialize(){}

    static bool get_face_rects(const char* jsonstr, fdrtsp::FaceRectVector &vct_face_rect)
    {
        //Reset
        vct_face_rect.clear();

        Json::Value obj;
        Json::CharReaderBuilder builder;
        builder["collectComments"] = false;
        Json::String errs;
        std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
        bool ok = reader->parse(jsonstr, jsonstr + strlen(jsonstr), &obj, &errs);
        if (!ok){
            std::cout << "Json parse failed, jsonstr=" << jsonstr << std::endl;
            return false;
        }

        if (!obj["face_rects"].isArray()) {
            std::cout << "Json format error!" << std::endl;
            return false;
        }

        for(int i =0; i< obj["face_rects"].size(); ++i) {
            Json::Value jsonRects = obj["face_rects"][i];
            fdrtsp::FaceRect rc;
            rc.x1 = jsonRects["x1"].asFloat();
            rc.y1 = jsonRects["y1"].asFloat();
            rc.x2 = jsonRects["x2"].asFloat();
            rc.y2 = jsonRects["y2"].asFloat();
            vct_face_rect.push_back(rc);
        }

        return 0;
    }
    static bool get_face_features(const char* jsonstr, fdrtsp::FaceRectVector& vct_face_rect,
            std::vector<fdrtsp::FaceFeature>& vct_features, std::vector<fdrtsp::FaceRecognitionInfo> &vct_label)
    {
        Json::Value obj;
        Json::CharReaderBuilder builder;
        builder["collectComments"] = false;
        Json::String errs;
        std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
        bool ok = reader->parse(jsonstr, jsonstr + strlen(jsonstr), &obj, &errs);
        if (!ok){
            std::cout << "Json parse failed, jsonstr=" << jsonstr << std::endl;
            return false;
        }

        if (obj.isNull()){
            return false;
        }

        if (!obj["face_rects"].isArray()) {
            std::cout << "Json format error!" << std::endl;
            return false;
        }

        for(int i =0; i< obj["face_rects"].size(); ++i) {
            Json::Value jsonRects = obj["face_rects"][i];
            fdrtsp::FaceRect rc;
            rc.x1 = jsonRects["x1"].asFloat();
            rc.y1 = jsonRects["y1"].asFloat();
            rc.x2 = jsonRects["x2"].asFloat();
            rc.y2 = jsonRects["y2"].asFloat();
            vct_face_rect.push_back(rc);

            if (!obj["face_features"].isNull() && obj["face_features"].isArray()) {
                Json::Value jsonFeatures = obj["face_features"][i];
                fdrtsp::FaceFeature feature;
                JsonUtil::JsonArrayToFloatVector(jsonFeatures, &feature);
                vct_features.push_back(feature);
            }

            if (!obj["labels"].isNull() && obj["labels"].isArray()) {
                Json::Value jsonLabel = obj["labels"][i];
                fdrtsp::FaceRecognitionInfo fri;
                fri.label = jsonLabel["label"].asString();
                fri.similar = jsonLabel["similar"].asFloat();
                vct_label.push_back(fri);
            }
        }

        return 0;
    }

    enum SeiMode {
        SeiFaceRect = 0,
        SeiFaceFeature = 1,
        SeiFaceLabel = 2,
        SeiAll = 3,
    };
    static std::string ImageFaceInfo_toString(fdrtsp::ImageFaceInfo &info, SeiMode mode = SeiFaceRect){
        Json::Value obj;

        for(int i = 0;i < info.vct_face_rect.size(); ++i) {
            Json::Value rect_obj;
            rect_obj["x1"]= info.vct_face_rect[i].x1;
            rect_obj["x2"]= info.vct_face_rect[i].x2;
            rect_obj["y1"] = info.vct_face_rect[i].y1;
            rect_obj["y2"] = info.vct_face_rect[i].y2;
            obj["face_rects"].append(rect_obj);

            if ((mode & 1) && info.vct_face_feature.size() > 0) {
                // add features
                Json::Value json_feature;
                json_feature = JsonUtil::FloatVectorToJsonArray(info.vct_face_feature[i]);
                obj["face_features"].append(json_feature);
            }

            if ((mode & 2) && info.vct_face_labels.size() > 0)
            {
                //add labels
                Json::Value json_label;
                json_label["label"] = info.vct_face_labels[i].label;
                json_label["similar"] = info.vct_face_labels[i].similar;
                obj["labels"].append(json_label);
            }
        }
        return JsonUtil::JsonValueToString(obj);
    }
};



#endif //FACEDEMOEXAMPLE_FACE_INFO_SERIALIZE_H
