#pragma once

#include <functional>

#include "imgui.h"
#include "graphics_renderer.h"

namespace Engine { namespace UIImpl {

    namespace ImplImGui {
        void ResetContext();
    }
    namespace EUIPanel
    {
        bool ImplRender(const char* headerName);
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