//
// Created by hsyuan on 2019-03-11.
//
#include "otl_string.h"
#include <iostream>

std::string String::base64_enc(const void *ptr, size_t sz)
{
    const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string encoded_str;
    char *p = (char*)ptr;
    unsigned char Tmp[4]={0};
    int LineLength=0;
    for (int i = 0; i < (sz / 3); i++) {
        Tmp[1] = *p++;
        Tmp[2] = *p++;
        Tmp[3] = *p++;
        encoded_str += EncodeTable[Tmp[1] >> 2];
        encoded_str += EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
        encoded_str += EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
        encoded_str += EncodeTable[Tmp[3] & 0x3F];
        LineLength += 4;

        if (LineLength == 76) {
            encoded_str += "\r\n";
            LineLength = 0;
        }
    }

    int Mod = sz % 3;
    if (Mod == 1) {
        Tmp[1] = *p++;
        encoded_str += EncodeTable[(Tmp[1] & 0xFC) >> 2];
        encoded_str += EncodeTable[((Tmp[1] & 0x03) << 4)];
        encoded_str += "==";
    }
    else if (Mod == 2) {
        Tmp[1] = *p++;
        Tmp[2] = *p++;
        encoded_str += EncodeTable[(Tmp[1] & 0xFC) >> 2];
        encoded_str += EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
        encoded_str += EncodeTable[((Tmp[2] & 0x0F) << 2)];
        encoded_str += "=";
    }

    return encoded_str;
}

std::string String::base64_dec(const void *data, size_t size)
{
    const char DecodeTable[] =
            {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    62, // '+'
                    0, 0, 0,
                    63, // '/'
                    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
                    0, 0, 0, 0, 0, 0, 0,
                    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
                    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
                    0, 0, 0, 0, 0, 0,
                    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
                    39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
            };

    int nValue;
    int i= 0;
    std::string decoded_str;
    const char *p = (char*)data;
    int DataByte = size;
    while (i < DataByte)
    {
        if (*p != '\r' && *p!='\n')
        {
            nValue = DecodeTable[*p++] << 18;
            nValue += DecodeTable[*p++] << 12;
            decoded_str+=(nValue & 0x00FF0000) >> 16;
            //out_bytes++;
            if (*p != '=')
            {
                nValue += DecodeTable[*p++] << 6;
                decoded_str+=(nValue & 0x0000FF00) >> 8;
                //out_bytes++;
                if (*p != '=')
                {
                    nValue += DecodeTable[*p++];
                    decoded_str+=nValue & 0x000000FF;
                    //out_bytes++;
                }
            }
            i += 4;
        }
        else// CB CR
        {
            p++;
            i++;
        }
    }
    return decoded_str;
}