#include "engine_extension.h"

using namespace Engine;

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
}

EUIPanel::EUIPanel(const EString& title) 
    : EUIField(title), fOpen(true), fWasJustClosed(false)
{
    
}

bool EUIPanel::OnRender() 
{
    return fOpen;   
}

void EUIPanel::OnRenderEnd() 
{
    
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
