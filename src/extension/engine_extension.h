#pragma once


#ifdef EWIN
    #ifdef EXTENSION_EXPORT
        #define E_EXTAPI __declspec(dllexport)
    #else
        #define E_EXTAPI __declspec(dllimport)
    #endif
#else
    #define E_EXTAPI
#endif


#ifdef __cplusplus
    #define EXTERN_C extern "C" 
#else
    #define EXTERN_C
#endif

#ifdef EWIN
#define EXTENSION_EXPORT_FUN EXTERN_C __declspec(dllexport)
#else
#define EXTENSION_EXPORT_FUN  EXTERN_C __attribute__((visibility("default")))
#endif


#define EXT_ENTRY EXTENSION_EXPORT_FUN void entry(const char* extensionName, Engine::EScene* scene)

#include "engine.h"
#include "prefix_shared.h"

#include "engine_extension_register.h"

#include "extension_manager.h"