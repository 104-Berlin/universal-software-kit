#include "editor.h"

using namespace Editor;
using namespace Engine;

EResourceView::EResourceView() 
    : EUIField("RESOURCE_MANAGER"), selectedResource(0)
{
    shared::StaticSharedContext::instance().Events().GetEventDispatcher().Connect<events::EResourceAddedEvent>([this](events::EResourceAddedEvent event){
        ERef<EResourceData> data = shared::GetResource(event.ResourceID);
        if (data)
        {
            fResources.push_back({data->ID, data->Name, data->PathToFile});
        }
    });
}

bool EResourceView::OnRender() 
{
    return true;
}
