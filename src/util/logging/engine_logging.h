#pragma once

namespace Engine {

	namespace Logging {

		enum class ELogType
		{
			ERR,
			INFO,
			WARN,
			DB,
			UNDO,
			ASSERT
		};


		extern EString GetPrefixString(ELogType type);
		extern EString GetFileNameFromPath(const EString& path);
		extern void Log(ELogType type, const EString& str, u32 line, const char* file);

	}

}


#define E_ERROR(x) ::Engine::Logging::Log(::Engine::Logging::ELogType::ERR, (x), __LINE__, __FILE__)
#define E_DEBUG(x) ::Engine::Logging::Log(::Engine::Logging::ELogType::DB, (x), __LINE__, __FILE__)
#define E_UNDO(x) ::Engine::Logging::Log(::Engine::Logging::ELogType::UNDO, (x), __LINE__, __FILE__)
#define E_WARN(x) ::Engine::Logging::Log(::Engine::Logging::ELogType::WARN, (x), __LINE__, __FILE__)
#define E_INFO(x) ::Engine::Logging::Log(::Engine::Logging::ELogType::INFO, (x), __LINE__, __FILE__)

#ifdef E_TEST
#define E_ASSERT_M(x, msg) if (!(x)) { ::Engine::Logging::Log(::Engine::Logging::ELogType::ASSERT, (msg), __LINE__, __FILE__); exit(1); } 
#else
#define E_ASSERT_M(x, msg) if (!(x)) { ::Engine::Logging::Log(::Engine::Logging::ELogType::ASSERT, (msg), __LINE__, __FILE__); } 
#endif
#define E_ASSERT(x) E_ASSERT_M(x, #x)