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



#ifdef EXT_RENDERER_ENABLED
#include "imgui.h"
#include "impl/engine_ui_impl_imgui.h"


#define INIT_IMGUI_FUNC EXTENSION_EXPORT_FUN void InitImGui()\
{\
    Engine::UIImpl::ImplImGui::ResetContext();\
}
#else
#define INIT_IMGUI_FUNC
#endif

#define EXT_ENTRY INIT_IMGUI_FUNC\
                    EXTENSION_EXPORT_FUN void entry(const char* extensionName, Engine::EExtInitInfo* extension)


#include "engine.h"
#include "prefix_shared.h"

#include "engine_extension_register.h"

#include "ui/engine_ext_ui.h"
#include "ui/engine_ui_register.h"

#include "extension_manager.h"