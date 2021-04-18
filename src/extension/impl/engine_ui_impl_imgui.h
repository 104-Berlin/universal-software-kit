#pragma once

#include <functional>

#include "imgui.h"
#include "ImGuizmo.h"
#include "graphics_renderer.h"

namespace Engine { namespace UIImpl {

    namespace ImplImGui {
        void ResetContext();
    }
    namespace EUIPanel
    {
        void ImplRender(const char* headerName, bool* open);
        void ImplRenderEnd();
    }

    class EImGuiViewport 
    {
    private:
        Graphics::GFrameBuffer* fFrameBuffer;
    public:
        EImGuiViewport();
        ~EImGuiViewport();

        void Render(std::function<void(Graphics::GContext*, Graphics::GFrameBuffer*)> renderFunction);
    };

} }