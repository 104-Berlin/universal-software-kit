#include "engine.h"
#include "engine_exception.h"

using namespace Engine;

ESocketException::ESocketException(EType type, const EString& message)
{
    fMessage = GetTypeString(type) + message;
}

const char* ESocketException::what() const noexcept 
{
    return fMessage.c_str();
}

const char* ESocketException::GetTypeString(EType type)
{
    switch (type)
    {
    case EType::Unknown:
        return "Unknown Error:";
    case EType::Socket:
        return "Socket Error:";
    case EType::Bind:
        return "Bind Error:";
    case EType::Listen:
        return "Listen Error:";
    case EType::Accept:
        return "Accept Error:";
    case EType::Connect:
        return "Connect Error:";
    case EType::Send:
        return "Send Error:";
    case EType::Receive:
        return "Receive Error:";
    case EType::Close:
        return "Close Error:";
    case EType::Shutdown:
        return "Shutdown Error:";
    case EType::GetHostByName:
        return "GetHostByName Error:";
    case EType::GetHostByAddr:
        return "GetHostByAddr Error:";
    }
}
