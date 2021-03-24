#include "engine_shared.h"
#include "prefix_ecs.h"

using namespace Engine;

EObject::EObject(EEntity handle, EScene* scene)
{
    Set(handle, scene);
}

EUUID EObject::GetUuid() const
{
    if (fScene->fRegistry.has<ETagComponent>(fHandle))
    {
        return GetComponent<ETagComponent>().UUID;
    }
    return EUUID();
}

void EObject::Set(EEntity entity, EScene* scene) 
{
    fHandle = entity;
    fScene = scene;
}

void EObject::Set(EObject object) 
{
    Set(object.fHandle, object.fScene);
}

void EObject::FromJsObject(const EJson& ref)
{
    for (ComponentDescription* dsc : EPanelComponentData::data().GetComponentDescription())
    {
        if (JSHelper::HasParam(ref, dsc->Name))
        {
            if (!dsc->Has(*this)) 
            { 
                dsc->Create(*this); 
            }
            dsc->FromJsObject(*this, ref[dsc->Name]);
        }
    }
    OnFromJsObject(ref);
}

void EObject::SetJsObject(EJson& ref) const
{
    for (ComponentDescription* compDsc : EPanelComponentData::data().GetComponentDescription())
    {
        if (compDsc->Has(*this))
        {
            compDsc->SetJsObject(*this, ref[compDsc->Name]);
        }   
    }
    OnSetJsObject(ref);
}

void EObject::OnFromJsObject(const EJson& ref)
{
    
}

void EObject::OnSetJsObject(EJson& ref) const
{
    
}


