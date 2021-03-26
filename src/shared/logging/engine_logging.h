#pragma once

namespace Engine {

	namespace Logging {
		static const std::chrono::system_clock::time_point g_AppStart = std::chrono::system_clock::now();
		
		enum ELogType
		{
			ERROR,
			INFO,
			WARN,
			DB,
			UNDO,
			ASSERT
		};

		static inline EString GetPrefixString(ELogType type)
		{
			std::chrono::duration<double> duration = std::chrono::system_clock::now() - g_AppStart;
			switch (type)
			{
			case Engine::Logging::ERROR:
				return "[ERROR]" + std::to_string(duration.count()) + "[";
				break;
			case Engine::Logging::ASSERT:
				return "[ASSERT]" + std::to_string(duration.count()) + "[";
				break;
			case Engine::Logging::INFO:
				return "[INFO]" + std::to_string(duration.count()) + "[";
				break;
			case Engine::Logging::WARN:
				return "[WARN]" + std::to_string(duration.count()) + "[";
				break;
			case Engine::Logging::DB:
				return "[DEBUG]" + std::to_string(duration.count()) + "[";
				break;
			case Engine::Logging::UNDO:
				return "[UNDO]" + std::to_string(duration.count()) + "[";
				break;
			default:
				break;
			}
			return "";
		}

		static inline EString GetFileNameFromPath(const EString& path)
		{
			size_t lastSlashPos = path.find_last_of('/');
			return path.substr(lastSlashPos + 1);
		}

		static inline void Log(ELogType type, const EString& str, u32 line, const char* file)
		{
			std::cout << GetPrefixString(type) << GetFileNameFromPath(file) << "l." << line << "]" << str << std::endl;
		}

	}

}


#define E_ERROR(x) ::Engine::Logging::Log(::Engine::Logging::ELogType::ERROR, (x), __LINE__, __FILE__)
#define E_DEBUG(x) ::Engine::Logging::Log(::Engine::Logging::ELogType::DB, (x), __LINE__, __FILE__)
#define E_UNDO(x) ::Engine::Logging::Log(::Engine::Logging::ELogType::UNDO, (x), __LINE__, __FILE__)
#define E_WARN(x) ::Engine::Logging::Log(::Engine::Logging::ELogType::WARN, (x), __LINE__, __FILE__)
#define E_INFO(x) ::Engine::Logging::Log(::Engine::Logging::ELogType::INFO, (x), __LINE__, __FILE__)

#define E_ASSERT(x, msg) if (!(x)) { ::Engine::Logging::Log(::Engine::Logging::ELogType::ASSERT, msg, __LINE__, __FILE__); } 