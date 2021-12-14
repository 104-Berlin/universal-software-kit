#pragma once

#include <imgui.h>

#include <functional>

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Common/interface/BasicMath.hpp"

#include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"



#ifdef EWIN
    #ifdef EXPORT_EDITOR_RENDERING
        #define E_EDREAPI __declspec(dllexport)
    #else
        #define E_EDREAPI __declspec(dllimport)
    #endif
#else
    #define E_EDREAPI
#endif


namespace editor_rendering
{
    static void SetImGuiContext(ImGuiContext* context)
    {
        ImGui::SetCurrentContext(context);
    }


    E_EDREAPI ImGuiContext* GetCurrentImGuiContext();

    E_EDREAPI void RunApplicationLoop(std::function<void()> initFunction, std::function<void()> RenderImGui, std::function<void()> CleanUp, void(*SetImGuiContext)(ImGuiContext*));
}