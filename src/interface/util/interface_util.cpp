#include "prefix_interface.h"

#ifdef EWIN
const DWORD MS_VC_EXCEPTION=0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // Must be 0x1000.
   LPCSTR szName; // Pointer to name (in user addr space).
   DWORD dwThreadID; // Thread ID (-1=caller thread).
   DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
#endif

using namespace Engine;

void PrintProperty_Prim(EProperty* primitive)
{
    EValueDescription dsc = primitive->GetDescription();
    const EString& primitiveId = dsc.GetId();
    if (primitiveId == E_TYPEID_STRING) { std::cout << "\"" << static_cast<EValueProperty<EString>*>(primitive)->GetValue() << "\""; } 
    else if (primitiveId == E_TYPEID_INTEGER) { std::cout << (static_cast<EValueProperty<i32>*>(primitive))->GetValue(); }
    else if (primitiveId == E_TYPEID_UNSIGNED_INTEGER) { std::cout << (static_cast<EValueProperty<u32>*>(primitive))->GetValue(); }
    else if (primitiveId == E_TYPEID_UNSIGNED_BIG_INTEGER) { std::cout << (static_cast<EValueProperty<u64>*>(primitive))->GetValue(); }
    else if (primitiveId == E_TYPEID_DOUBLE) { std::cout << (static_cast<EValueProperty<double>*>(primitive))->GetValue(); }
    else if (primitiveId == E_TYPEID_BOOL) { std::cout << (static_cast<EValueProperty<bool>*>(primitive))->GetValue(); }
}

void PrintProperty_Struct(EStructProperty* prop)
{
    EValueDescription dsc = prop->GetDescription();
    std::cout << std::endl;
    for (auto& entry : dsc.GetStructFields())
    {
        EProperty* childProp = prop->GetProperty(entry.first);
        if (childProp)
        {
            std::cout << "\t";
            inter::PrintProperty(childProp);
        }
    }
}

void PrintProperty_Array(EArrayProperty* prop)
{
    std::cout << "[";
    for (auto& entry : prop->GetElements())
    {
        inter::PrintProperty(entry);
        std::cout << ",";
    }
    std::cout << "]";
}

void inter::PrintProperty(EProperty* prop) 
{
    EValueDescription dsc = prop->GetDescription();
    std::cout << "\"" << prop->GetPropertyName() << "\": ";
    
    switch (dsc.GetType())
    {
    case EValueType::PRIMITIVE: PrintProperty_Prim(prop); break;
    case EValueType::ARRAY: PrintProperty_Array(static_cast<EArrayProperty*>(prop)); break;
    case EValueType::STRUCT: PrintProperty_Struct(static_cast<EStructProperty*>(prop)); break;
    case EValueType::ENUM: break;
    case EValueType::UNKNOWN: break;
    }
    
    std::cout << std::endl;
}


#ifdef EWIN
    void SetThreadName(uint32_t dwThreadID, const char* threadName)
    {

    // DWORD dwThreadID = ::GetThreadId( static_cast<HANDLE>( t.native_handle() ) );

    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = threadName;
    info.dwThreadID = dwThreadID;
    info.dwFlags = 0;

    __try
    {
        RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
    }
    }
    void inter::SetCurrentThreadName( const EString& name)
    {
        SetThreadName(GetCurrentThreadId(),name.c_str());
    }

#else
    void inter::SetCurrentThreadName( const EString& name)
    {
#ifdef EMAC
        pthread_setname_np(name.c_str());
#else
        prctl(PR_SET_NAME,name.c_str(),0,0,0);
#endif
    }

#endif


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

EString inter::EncodeBase64(const u8* data, size_t dataLen) 
{
    EString result;
    for (size_t i = 0; i < dataLen; i += 3)
    {
        result += ConstructBase64FromTripple(data[i], data[i + 1], data[i + 2]);
    }
    return result;
}

bool IsBase64(const EString& base64)
{
    for (size_t i = 0; i < base64.length(); i++)
    {
        if (!isalnum(base64[i]) && base64[i] != '+' && base64[i] != '/')
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

bool inter::DecodeBase64(const EString& base64, u8** data, size_t* dataLen) 
{
    if (!IsBase64(base64)) { return false; }
    size_t outLen = (base64.length() / 4) * 3;
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

        newData[currIndex++] = byteA;
        newData[currIndex++] = byteB;
        newData[currIndex++] = byteC;
    }
    *data = newData;
    *dataLen = outLen;
    return true;
}