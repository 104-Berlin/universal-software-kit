#include "engine_extension.h"

using namespace Engine;

void intern::InitUI()
{
#ifdef EXT_RENDERER_ENABLED
    UIImpl::ImplImGui::ResetContext();
#endif
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
#ifdef EXT_RENDERER_ENABLED
    UIImpl::EUIPanel::ImplRender(GetLabel().c_str(), &fOpen);
#endif
    return fOpen;
}

void EUIPanel::OnRenderEnd() 
{
#ifdef EXT_RENDERER_ENABLED
    UIImpl::EUIPanel::ImplRenderEnd();
#endif
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
    
}

#ifdef EXT_RENDERER_ENABLED
void EUIViewport::SetRenderFunction(RenderFunction renderFunction) 
{
    fRenderFuntion = renderFunction;
}
#endif

bool EUIViewport::OnRender() 
{
#ifdef EXT_RENDERER_ENABLED
    fImGuiViewport.Render(fRenderFuntion);
#endif
    return true;
}


EUIButton::EUIButton(const EString& label) 
    : EUIField(label)
{
    
}

bool EUIButton::OnRender() 
{
#ifdef EXT_RENDERER_ENABLED
    if (UIImpl::EUIButton::ImplRender(GetLabel().c_str()))
    {
        fEventDispatcher.Enqueue<EClickEvent>({0,0});
    }
#endif
    return true;
}
