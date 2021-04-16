#include "engine_ui_impl_imgui.h"


#include "imgui.h"

using namespace Engine;
using namespace UIImpl;

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