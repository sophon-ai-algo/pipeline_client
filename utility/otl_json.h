
#ifndef OTL_JSON_H_
#define OTL_JSON_H_

#include <string>
#include <iostream>
#include <vector>
#include "json/json.h"

///////////////////////////////////////////////////////////////////////////////
// JSON Helpers
///////////////////////////////////////////////////////////////////////////////
namespace {
    template<typename T>
    bool JsonArrayToVector(const Json::Value &value,
                           bool (*getter)(const Json::Value &in, T *out),
                           std::vector<T> *vec) {
        vec->clear();
        if (!value.isArray()) {
            return false;
        }

        for (Json::Value::ArrayIndex i = 0; i < value.size(); ++i) {
            T val;
            if (!getter(value[i], &val)) {
                return false;
            }
            vec->push_back(val);
        }

        return true;
    }

// Trivial getter helper
    bool GetValueFromJson(const Json::Value &in, Json::Value *out) {
        *out = in;
        return true;
    }
}  // unnamed namespace

namespace {
    template<typename T>
    Json::Value VectorToJsonArray(const std::vector<T> &vec) {
        Json::Value result(Json::arrayValue);
        for (size_t i = 0; i < vec.size(); ++i) {
            result.append(Json::Value(vec[i]));
        }
        return result;
    }
}  // unnamed namespace

class JsonUtil {
public:
    JsonUtil() {}

    ~JsonUtil() {}

    static bool GetStringFromJson(const Json::Value &in, std::string *out) {
        if (!in.isString()) {
            std::ostringstream s;
            if (in.isBool()) {
                s << std::boolalpha << in.asBool();
            } else if (in.isInt()) {
                s << in.asInt();
            } else if (in.isUInt()) {
                s << in.asUInt();
            } else if (in.isDouble()) {
                s << in.asDouble();
            } else {
                return false;
            }
            *out = s.str();
        } else {
            *out = in.asString();
        }
        return true;
    }

    static bool GetIntFromJson(const Json::Value &in, int *out) {
        bool ret;
        if (!in.isString()) {
            ret = in.isConvertibleTo(Json::intValue);
            if (ret) {
                *out = in.asInt();
            }
        } else {
            long val;  // NOLINT
            const char *c_str = in.asCString();
            char *end_ptr;
            errno = 0;
            val = strtol(c_str, &end_ptr, 10);  // NOLINT
            ret = (end_ptr != c_str && *end_ptr == '\0' && !errno &&
                   val >= INT_MIN && val <= INT_MAX);
            *out = val;
        }
        return ret;
    }

    static bool GetUIntFromJson(const Json::Value &in, unsigned int *out) {
        bool ret;
        if (!in.isString()) {
            ret = in.isConvertibleTo(Json::uintValue);
            if (ret) {
                *out = in.asUInt();
            }
        } else {
            unsigned long val;  // NOLINT
            const char *c_str = in.asCString();
            char *end_ptr;
            errno = 0;
            val = strtoul(c_str, &end_ptr, 10);  // NOLINT
            ret = (end_ptr != c_str && *end_ptr == '\0' && !errno &&
                   val <= UINT_MAX);
            *out = val;
        }
        return ret;
    }

    static bool GetBoolFromJson(const Json::Value &in, bool *out) {
        bool ret;
        if (!in.isString()) {
            ret = in.isConvertibleTo(Json::booleanValue);
            if (ret) {
                *out = in.asBool();
            }
        } else {
            if (in.asString() == "true") {
                *out = true;
                ret = true;
            } else if (in.asString() == "false") {
                *out = false;
                ret = true;
            } else {
                ret = false;
            }
        }
        return ret;
    }

    static bool GetDoubleFromJson(const Json::Value &in, double *out) {
        bool ret;
        if (!in.isString()) {
            ret = in.isConvertibleTo(Json::realValue);
            if (ret) {
                *out = in.asDouble();
            }
        } else {
            double val;
            const char *c_str = in.asCString();
            char *end_ptr;
            errno = 0;
            val = strtod(c_str, &end_ptr);
            ret = (end_ptr != c_str && *end_ptr == '\0' && !errno);
            *out = val;
        }
        return ret;
    }

    static bool GetFloatFromJson(const Json::Value &in, float *out) {
        bool ret;
        if (!in.isString()) {
            ret = in.isConvertibleTo(Json::realValue);
            if (ret) {
                *out = in.asFloat();
            }
        } else {
            float val;
            const char *c_str = in.asCString();
            char *end_ptr;
            errno = 0;
            val = strtof(c_str, &end_ptr);
            ret = (end_ptr != c_str && *end_ptr == '\0' && !errno);
            *out = val;
        }
        return ret;
    }

    static bool JsonArrayToValueVector(const Json::Value &in,
                                       std::vector<Json::Value> *out) {
        return JsonArrayToVector(in, GetValueFromJson, out);
    }

    static bool JsonArrayToIntVector(const Json::Value &in,
                                     std::vector<int> *out) {
        return JsonArrayToVector(in, JsonUtil::GetIntFromJson, out);
    }

    static bool JsonArrayToUIntVector(const Json::Value &in,
                                      std::vector<unsigned int> *out) {
        return JsonArrayToVector(in, JsonUtil::GetUIntFromJson, out);
    }

    static bool JsonArrayToStringVector(const Json::Value &in,
                                        std::vector<std::string> *out) {
        return JsonArrayToVector(in, JsonUtil::GetStringFromJson, out);
    }

    static bool JsonArrayToBoolVector(const Json::Value &in,
                                      std::vector<bool> *out) {
        return JsonArrayToVector(in, JsonUtil::GetBoolFromJson, out);
    }

    static bool JsonArrayToDoubleVector(const Json::Value &in,
                                        std::vector<double> *out) {
        return JsonArrayToVector(in, JsonUtil::GetDoubleFromJson, out);
    }

    static bool JsonArrayToFloatVector(const Json::Value &in,
                                       std::vector<float> *out) {
        return JsonArrayToVector(in, JsonUtil::GetFloatFromJson, out);
    }


    static Json::Value ValueVectorToJsonArray(const std::vector<Json::Value> &in) {
        return VectorToJsonArray(in);
    }

    static Json::Value IntVectorToJsonArray(const std::vector<int> &in) {
        return VectorToJsonArray(in);
    }

    static Json::Value UIntVectorToJsonArray(const std::vector<unsigned int> &in) {
        return VectorToJsonArray(in);
    }

    static Json::Value StringVectorToJsonArray(const std::vector<std::string> &in) {
        return VectorToJsonArray(in);
    }

    static Json::Value BoolVectorToJsonArray(const std::vector<bool> &in) {
        return VectorToJsonArray(in);
    }

    static Json::Value DoubleVectorToJsonArray(const std::vector<double> &in) {
        return VectorToJsonArray(in);
    }

    static Json::Value FloatVectorToJsonArray(const std::vector<float> &in) {
        return VectorToJsonArray(in);
    }

    static bool GetValueFromJsonArray(const Json::Value &in, size_t n,
                                      Json::Value *out) {
        if (!in.isArray() || !in.isValidIndex(static_cast<int>(n))) {
            return false;
        }

        *out = in[static_cast<Json::Value::ArrayIndex>(n)];
        return true;
    }

    static bool GetIntFromJsonArray(const Json::Value &in, size_t n,
                                    int *out) {
        Json::Value x;
        return GetValueFromJsonArray(in, n, &x) && JsonUtil::GetIntFromJson(x, out);
    }

    static bool GetUIntFromJsonArray(const Json::Value &in, size_t n,
                                     unsigned int *out) {
        Json::Value x;
        return GetValueFromJsonArray(in, n, &x) && JsonUtil::GetUIntFromJson(x, out);
    }

    static bool GetStringFromJsonArray(const Json::Value &in, size_t n,
                                       std::string *out) {
        Json::Value x;
        return GetValueFromJsonArray(in, n, &x) && JsonUtil::GetStringFromJson(x, out);
    }

    static bool GetBoolFromJsonArray(const Json::Value &in, size_t n,
                                     bool *out) {
        Json::Value x;
        return GetValueFromJsonArray(in, n, &x) && JsonUtil::GetBoolFromJson(x, out);
    }

    static bool GetDoubleFromJsonArray(const Json::Value &in, size_t n,
                                       double *out) {
        Json::Value x;
        return GetValueFromJsonArray(in, n, &x) && JsonUtil::GetDoubleFromJson(x, out);
    }

    static bool GetValueFromJsonObject(const Json::Value &in, const std::string &k,
                                       Json::Value *out) {
        if (!in.isObject() || !in.isMember(k)) {
            return false;
        }

        *out = in[k];
        return true;
    }

    static bool GetIntFromJsonObject(const Json::Value &in, const std::string &k,
                                     int *out) {
        Json::Value x;
        return GetValueFromJsonObject(in, k, &x) && JsonUtil::GetIntFromJson(x, out);
    }

    static bool GetUIntFromJsonObject(const Json::Value &in, const std::string &k,
                                      unsigned int *out) {
        Json::Value x;
        return GetValueFromJsonObject(in, k, &x) && JsonUtil::GetUIntFromJson(x, out);
    }

    static bool GetStringFromJsonObject(const Json::Value &in, const std::string &k,
                                        std::string *out) {
        Json::Value x;
        return GetValueFromJsonObject(in, k, &x) && JsonUtil::GetStringFromJson(x, out);
    }

    static bool GetBoolFromJsonObject(const Json::Value &in, const std::string &k,
                                      bool *out) {
        Json::Value x;
        return GetValueFromJsonObject(in, k, &x) && JsonUtil::GetBoolFromJson(x, out);
    }

    static bool GetDoubleFromJsonObject(const Json::Value &in, const std::string &k,
                                        double *out) {
        Json::Value x;
        return GetValueFromJsonObject(in, k, &x) && JsonUtil::GetDoubleFromJson(x, out);
    }

    static std::string JsonValueToString(const Json::Value &json) {
        Json::StreamWriterBuilder builder;
        builder["precision"]=2;
        builder["precisionType"]="decimal";
        builder["indentation"] = "";
        return Json::writeString(builder, json);
    }

};  // end of JsonUtil


#endif  // OTL_JSON_H_
