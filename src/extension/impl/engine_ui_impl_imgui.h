#pragma once


namespace Engine { namespace UIImpl {

    namespace ImplImGui {
        void ResetContext();
    }
    namespace EUIPanel
    {
        bool ImplRender(const char* headerName);
        void ImplRenderEnd();
    }

} }