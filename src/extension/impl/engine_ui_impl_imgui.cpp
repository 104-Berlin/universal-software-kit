#include "engine_ui_impl_imgui.h"


using namespace Engine;
using namespace UIImpl;

void ImplImGui::ResetContext()
{
    Graphics::Wrapper::SetImGuiContext(Graphics::Wrapper::GetCurrentImGuiContext());
    ImGui::SetCurrentContext(Graphics::Wrapper::GetCurrentImGuiContext());
}

void EUIPanel::ImplRender(const char* headerName, bool* open)
{
    ImGui::Begin(headerName, open);
}

void EUIPanel::ImplRenderEnd()
{
    ImGui::End();
}

EImGuiViewport::EImGuiViewport() 
{
    fFrameBuffer = Graphics::Wrapper::CreateFrameBuffer();
}

EImGuiViewport::~EImGuiViewport() 
{
    delete fFrameBuffer;   
}

void EImGuiViewport::Render(std::function<void(Graphics::GContext*, Graphics::GFrameBuffer*)> renderFunction) 
{
    ImVec2 contentRegion = ImGui::GetContentRegionAvail();

    fFrameBuffer->Resize(contentRegion.x, contentRegion.y, Graphics::GFrameBufferFormat::RGBA8);
    fFrameBuffer->Bind();
    if (renderFunction)
    {
        renderFunction(Graphics::Wrapper::GetMainContext(), fFrameBuffer);
    }
    fFrameBuffer->Unbind();

    ImGui::Image((ImTextureID)(unsigned long)fFrameBuffer->GetColorAttachment(), contentRegion, {0, 1}, {1, 0});
}