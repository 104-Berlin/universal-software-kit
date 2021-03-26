#include "engine.h"
#include "prefix_shared.h"

namespace Engine {
    static const std::chrono::system_clock::time_point g_AppStart = std::chrono::system_clock::now();


    EString Logging::GetPrefixString(Logging::ELogType type)
    {
        std::chrono::duration<double> duration = std::chrono::system_clock::now() - g_AppStart;
        switch (type)
        {
        case Engine::Logging::ELogType::ERR:
            return "[ERROR]" + std::to_string(duration.count()) + "[";
            break;
        case Engine::Logging::ELogType::ASSERT:
            return "[ASSERT]" + std::to_string(duration.count()) + "[";
            break;
        case Engine::Logging::ELogType::INFO:
            return "[INFO]" + std::to_string(duration.count()) + "[";
            break;
        case Engine::Logging::ELogType::WARN:
            return "[WARN]" + std::to_string(duration.count()) + "[";
            break;
        case Engine::Logging::ELogType::DB:
            return "[DEBUG]" + std::to_string(duration.count()) + "[";
            break;
        case Engine::Logging::ELogType::UNDO:
            return "[UNDO]" + std::to_string(duration.count()) + "[";
            break;
        default:
            break;
        }
        return "";
    }


    EString Logging::GetFileNameFromPath(const EString& path)
    {
        size_t lastSlashPos = path.find_last_of('/');
        return path.substr(lastSlashPos + 1);
    }

    void Logging::Log(Logging::ELogType type, const EString& str, u32 line, const char* file)
    {
        std::cout << GetPrefixString(type) << GetFileNameFromPath(file) << "l." << line << "]" << str << std::endl;
    }

}