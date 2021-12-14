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


#define INIT_IMGUI_FUNC EXTENSION_EXPORT_FUN void InitImGui() { ImGui::SetCurrentContext(editor_rendering::GetCurrentImGuiContext()); }

#define APP_ENTRY INIT_IMGUI_FUNC\
                  EXTENSION_EXPORT_FUN void app_entry(const char* extensionName, Engine::EAppInit info)


#define APP_CLEANUP EXTENSION_EXPORT_FUN void app_cleanup()


#include "editor_icons_material_design.h"
#include "graphics_wrapper.h"
#include "graphics_renderer.h"

#include "editor_rendering.h"


typedef Diligent::Vector2<double> EVec2;
typedef Diligent::Vector3<double> EVec3;
typedef Diligent::Vector4<double> EVec4;

typedef Diligent::Matrix4x4<double> EMat4;

#include "default-types/editor_def_res_image.h"


#include "editor_vector_dsc.h"

#include "ui/editor_ext_ui.h"
#include "ui/editor_ext_viewport_tools.h"
#include "ui/editor_ext_viewport.h"
#include "ui/editor_ui_register.h"