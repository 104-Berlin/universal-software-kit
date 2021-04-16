#include "engine_ui_impl_imgui.h"


#include "imgui.h"
#include "graphics_wrapper.h"

using namespace Engine;
using namespace UIImpl;

void ImplImGui::ResetContext()
{
    Graphics::Wrapper::SetImGuiContext(Graphics::Wrapper::GetCurrentImGuiContext());
    ImGui::SetCurrentContext(Graphics::Wrapper::GetCurrentImGuiContext());
}

bool EUIPanel::ImplRender(const char* headerName)
{
    bool open = true;
    ImGui::Begin(headerName, &open);
    return open;
}

void EUIPanel::ImplRenderEnd()
{
    ImGui::End();
}