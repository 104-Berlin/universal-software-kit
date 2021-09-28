#include "prefix_util.h"

using namespace Engine;


// BASE64 Encoding and Decoding
const char* BASE_64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

EString ConstructBase64FromTripple(const u8& byteA, const u8& byteB, const u8& byteC)
{
    u32 bit_24_val = 0;
    bit_24_val += (byteA << 16) + (byteB << 8) + byteC;
    u8 first = (bit_24_val >> 18) & 63;
    u8 second = (bit_24_val >> 12) & 63;
    u8 third = (bit_24_val >> 6) & 63;
    u8 fourth = bit_24_val & 63;

    return EString({BASE_64_CHARS[first], BASE_64_CHARS[second], BASE_64_CHARS[third], BASE_64_CHARS[fourth]});
}

EString Base64::Encode(const u8* data, size_t dataLen) 
{
    EString result;
    for (size_t i = 0; i < dataLen; i += 3)
    {
        result += ConstructBase64FromTripple(data[i], data[i + 1], data[i + 2]);
    }
    // Add padding 
    for (size_t i = 0; i < (dataLen % 3); i++)
    {
        result += "=";
    }
    return result;
}

bool IsBase64(const EString& base64)
{
    for (size_t i = 0; i < base64.length(); i++)
    {
        if (!isalnum(base64[i]) && base64[i] != '+' && base64[i] != '/' && base64[i] != '=')
        {
            return false;
        }
    }
    return true;
}

u8 Base64CharToIndex(const char base64)
{
    if (base64 >= 'A' && base64 <= 'Z') { return base64 - 'A'; }
    if (base64 >= 'a' && base64 <= 'z') { return base64 - 'a' + 26; }
    if (base64 >= '0' && base64 <= '9') { return base64 - '0' + 52; }
    if (base64 == '+') { return 62; }
    if (base64 == '/') { return 63; }
    return 0;
}

bool Base64::Decode(const EString& base64, u8** data, size_t* dataLen) 
{
    if (!IsBase64(base64)) { return false; }
    size_t padding = 0;
    while (base64[(base64.length() - 1) - padding] == '=')
    {
        padding++;
    }
    

    size_t outLen = (base64.length() / 4) * 3 - (padding ? (3 - padding) : 0);
    u8* newData = new u8[outLen];
    size_t currIndex = 0;


    
    for (size_t i = 0; i < base64.length(); i += 4)
    {
        u8 first = Base64CharToIndex(base64[i]);
        u8 second = Base64CharToIndex(base64[i + 1]);
        u8 third = Base64CharToIndex(base64[i + 2]);
        u8 fourth = Base64CharToIndex(base64[i + 3]);

        u32 bit_24_val = (first << 18) + (second << 12) + (third << 6) + (fourth);

        u8 byteA = (bit_24_val >> 16) & 255;
        u8 byteB = (bit_24_val >> 8) & 255;
        u8 byteC = (bit_24_val & 255);

        if (currIndex < outLen) { newData[currIndex++] = byteA; }
        if (currIndex < outLen) { newData[currIndex++] = byteB; }
        if (currIndex < outLen) { newData[currIndex++] = byteC; }
    }
    *data = newData;
    *dataLen = outLen;
    return true;
}