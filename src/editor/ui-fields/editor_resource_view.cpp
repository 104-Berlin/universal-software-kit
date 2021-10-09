#include "editor.h"

using namespace Editor;
using namespace Engine;

EResourceView::EResourceView() 
    : EUIField("RESOURCE_MANAGER"), selectedResource(0)
{
    shared::StaticSharedContext::instance().Events().GetEventDispatcher().Connect<events::EResourceAddedEvent>([this](events::EResourceAddedEvent event){
        fResources.push_back({event.ResourceID, event.Name, event.PathToFile});
    });

    EWeakRef<EUIField> resourceTypeContainer = AddChild(EMakeRef<EUIContainer>());
    resourceTypeContainer.lock()->SetWidth(150);
    
    resourceTypeContainer.lock()->SetCustomUpdateFunction([resourceTypeContainer](){
        if (resourceTypeContainer.expired()) { return; }
        resourceTypeContainer.lock()->Clear();
        
    });
}
