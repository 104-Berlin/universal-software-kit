#include "editor.h"

using namespace Editor;
using namespace Engine;

EResourceView::EResourceView() 
    : EUIField("RESOURCE_MANAGER"), fLastSelected(nullptr)
{

    EWeakRef<EUIField> resourceTypeContainer = AddChild(EMakeRef<EUIContainer>());
    AddChild(EMakeRef<EUISameLine>());
    EWeakRef<EUIField> resourceList = AddChild(EMakeRef<EUIContainer>());
    


    resourceTypeContainer.lock()->SetWidth(150);
    resourceTypeContainer.lock()->SetDirty();
    

    resourceTypeContainer.lock()->SetCustomUpdateFunction([this, resourceTypeContainer, resourceList](){
        if (resourceTypeContainer.expired()) { return; }
        resourceTypeContainer.lock()->Clear();
        fLastSelected = nullptr;
        auto allResources = shared::ExtensionManager().GetResourceRegister().GetAllItems();
        for (auto res : allResources)
        {
            EWeakRef<EUIField> selectField = resourceTypeContainer.lock()->AddChild(EMakeRef<EUISelectable>(res.ResourceName));
            selectField.lock()->AddEventListener<events::ESelectionChangeEvent>([this, res, selectField, resourceList](events::ESelectionChangeEvent event){
                if (event.Selected)
                {
                    if (fLastSelected)
                    {
                        static_cast<EUISelectable*>(fLastSelected)->SetSelected(false);
                    }
                    fLastSelected = selectField.lock().get();
                    fSelectedResourceType = res.ResourceName;
                }
                else
                {
                    fLastSelected = nullptr;
                    fSelectedResourceType = "";
                }
                if (!resourceList.expired())
                {
                    resourceList.lock()->SetDirty();
                }
            });
        }
    });

    shared::Events().Connect<events::EExtensionLoadedEvent>([resourceTypeContainer](){
        if (resourceTypeContainer.expired()) { return; }
        resourceTypeContainer.lock()->SetDirty();
    }, resourceTypeContainer.lock().get());

    shared::Events().Connect<events::EExtensionUnloadEvent>([resourceTypeContainer](){
        if (resourceTypeContainer.expired()) { return; }
        resourceTypeContainer.lock()->SetDirty();
    }, resourceTypeContainer.lock().get());


    resourceList.lock()->SetDirty();
    resourceList.lock()->SetCustomUpdateFunction([this, resourceList](){
        if (resourceList.expired()) { return; }
        resourceList.lock()->Clear();

        if (fSelectedResourceType.empty()) { return; }
        for (const Resource& res : fResources[fSelectedResourceType])
        {
            resourceList.lock()->AddChild(EMakeRef<EUISelectable>(res.Name)).lock()->SetTooltip(EMakeRef<EUILabel>(std::to_string(res.ID)));
        }
    });


    shared::Events().Connect<events::EResourceAddedEvent>([this, resourceList](events::EResourceAddedEvent event){
        fResources[event.ResourceType].push_back({event.ResourceID, event.Name, event.PathToFile});
        if (event.ResourceType == fSelectedResourceType && !resourceList.expired()) 
        { 
            resourceList.lock()->SetDirty();
        }
    }, this);
}
