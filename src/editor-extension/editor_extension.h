#pragma once

#ifdef EWIN
    #ifdef EXPORT_EDITOR_EXTENSION
        #define E_EDEXAPI __declspec(dllexport)
    #else
        #define E_EDEXAPI __declspec(dllimport)
    #endif
#else
    #define E_EDEXAPI
#endif



#include "engine.h"
#include "prefix_shared.h"
#include "engine_extension.h"


#define INIT_IMGUI_FUNC EXTENSION_EXPORT_FUN void InitImGui() { ImGui::SetCurrentContext(Graphics::Wrapper::GetCurrentImGuiContext()); }

#define APP_ENTRY INIT_IMGUI_FUNC\
                  EXTENSION_EXPORT_FUN void app_entry(const char* extensionName, Engine::EAppInit info)





#include "graphics_wrapper.h"
#include "graphics_renderer.h"

#include "ui/editor_ext_ui.h"
#include "ui/editor_ui_register.h"