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
    else if (primitiveId == E_TYPEID_FLOAT) { std::cout << (static_cast<EValueProperty<float>*>(primitive))->GetValue(); }
    else if (primitiveId == E_TYPEID_BOOL) { std::cout << (static_cast<EValueProperty<bool>*>(primitive))->GetValue(); }
}

void PrintProperty_Struct(EStructProperty* prop)
{
    EValueDescription dsc = prop->GetDescription();
    std::cout << std::endl;
    for (auto& entry : dsc.GetStructFields())
    {
        ERef<EProperty> childProp = prop->GetProperty(entry.first);
        if (childProp)
        {
            std::cout << "\t";
            inter::PrintProperty(childProp.get());
        }
    }
}

void PrintProperty_Array(EArrayProperty* prop)
{
    std::cout << "[";
    for (auto& entry : prop->GetElements())
    {
        inter::PrintProperty(entry.get());
        std::cout << ",";
    }
    std::cout << "]";
}

void inter::PrintProperty(EProperty* prop) 
{
    EJson propertyJson = ESerializer::WritePropertyToJs(prop);
    std::cout << "\"" << prop->GetPropertyName() << "\": " << propertyJson.dump(1) << std::endl;
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