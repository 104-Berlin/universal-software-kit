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
#include "prefix_interface.h"

#include "bx/bounds.h"
#include "bx/pixelformat.h"
#include "bx/string.h"
#include "bgfx/bgfx.h"
#include "bimg/bimg.h"
#include "dear-imgui/imgui.h"
#include "dear-imgui/imgui_internal.h"


#define INIT_IMGUI_FUNC EXTENSION_EXPORT_FUN void InitImGui() { ImGui::SetCurrentContext(Graphics::Wrapper::GetCurrentImGuiContext()); }

#define APP_ENTRY INIT_IMGUI_FUNC\
                  EXTENSION_EXPORT_FUN void app_entry(const char* extensionName, Engine::EAppInit info)


#define APP_CLEANUP EXTENSION_EXPORT_FUN void app_cleanup()


#include "default-types/editor_def_res_image.h"


#include "editor_vector_dsc.h"

#include "ui/editor_ext_ui.h"
#include "ui/editor_ext_viewport_tools.h"
#include "ui/editor_ext_viewport.h"
#include "ui/editor_ui_register.h"