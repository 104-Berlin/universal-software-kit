#include "editor_extension.h"

using namespace Engine;

void intern::InitUI()
{
    Graphics::Wrapper::SetImGuiContext(Graphics::Wrapper::GetCurrentImGuiContext());
    ImGui::SetCurrentContext(Graphics::Wrapper::GetCurrentImGuiContext());
}

EUIField::EUIField(const EString& label) 
    : fLabel(label), fID(next_ui_id()), fVisible(true), fDirty(false)
{
    
}

void EUIField::AddChild(const ERef<EUIField>& child) 
{
    fChildren.push_back(child);
}

void EUIField::SetCustomUpdateFunction(UpdateFunction function) 
{
    fCustomUpdateFunction = function;
}

void EUIField::Render() 
{
    if (!fVisible) { return; }

    if (fDirty && fCustomUpdateFunction)
    {
        fCustomUpdateFunction();
    }
    if (OnRender())
    {
        for (ERef<EUIField> uiField : fChildren)
        {
            uiField->Render();
        }
    }
    OnRenderEnd();
}

bool EUIField::OnRender() 
{
    return true;
}

void EUIField::OnRenderEnd() 
{
    
}

void EUIField::UpdateEventDispatcher() 
{
    OnUpdateEventDispatcher();
    fEventDispatcher.Update();

    for (ERef<EUIField> child : fChildren)
    {
        child->UpdateEventDispatcher();
    }
}

const EString& EUIField::GetLabel() const
{
    return fLabel;    
}

EUIPanel::EUIPanel(const EString& title) 
    : EUIField(title), fOpen(true)
{
    
}

bool EUIPanel::OnRender() 
{
    ImGui::Begin(GetLabel().c_str(), &fOpen);
    return fOpen;
}

void EUIPanel::OnRenderEnd() 
{
    ImGui::End();
}

bool EUIPanel::IsOpen() const
{
    return fOpen;
}

void EUIPanel::Close() 
{
    fOpen = false;
}

void EUIPanel::Open() 
{
    fOpen = true;
}

EUIViewport::EUIViewport() 
    : EUIField("VIEWPORT")
{
    fFrameBuffer = Graphics::Wrapper::CreateFrameBuffer();
}

EUIViewport::~EUIViewport() 
{
    delete fFrameBuffer;   
}

void EUIViewport::SetRenderFunction(RenderFunction renderFunction) 
{
    fRenderFuntion = renderFunction;
}

bool EUIViewport::OnRender() 
{
    ImVec2 contentRegion = ImGui::GetContentRegionAvail();

    fFrameBuffer->Resize(contentRegion.x, contentRegion.y, Graphics::GFrameBufferFormat::RGBA8);
    fFrameBuffer->Bind();
    if (fRenderFuntion)
    {
        fRenderFuntion(Graphics::Wrapper::GetMainContext(), fFrameBuffer);
    }
    fFrameBuffer->Unbind();

    ImGui::Image((ImTextureID)(unsigned long)fFrameBuffer->GetColorAttachment(), contentRegion, {0, 1}, {1, 0});

    return true;
}


EUIButton::EUIButton(const EString& label) 
    : EUIField(label)
{
    
}

bool EUIButton::OnRender() 
{
    if (ImGui::Button(GetLabel().c_str()))
    {
        fEventDispatcher.Enqueue<EClickEvent>({0,0});
    }
    return true;
}
