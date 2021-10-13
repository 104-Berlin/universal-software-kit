#include "editor.h"

using namespace Editor;
using namespace Engine;

EResourceView::EResourceView() 
    : EUIField("RESOURCE_MANAGER"), fLastSelected(nullptr), fPreviewSize(64.0f)
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
        EWeakRef<EUIField> changeSize = resourceList.lock()->AddChild(EMakeRef<EUIFloatEdit>("Size"));
        static_cast<EUIFloatEdit*>(changeSize.lock().get())->SetValue(fPreviewSize);
        EWeakRef<EUIField> resourceGrid = resourceList.lock()->AddChild(EMakeRef<EUIGrid>(fPreviewSize));


        changeSize.lock()->AddEventListener<events::EFloatChangeEvent>([this, resourceGrid](events::EFloatChangeEvent event){
            if (resourceGrid.expired()) { return; }
            static_cast<EUIGrid*>(resourceGrid.lock().get())->SetCellSize(event.Value);
            for (EWeakRef<EUIField> gridItem : resourceGrid.lock()->GetChildren())
            {
                gridItem.lock()->SetWidth(event.Value);
                gridItem.lock()->SetHeight(event.Value);
            }
        });

        if (fSelectedResourceType.empty()) { return; }
        for (const Resource& res : fResources[fSelectedResourceType])
        {
            EWeakRef<EUIField> resourceField = resourceGrid.lock()->AddChild(EMakeRef<EUIButton>(res.Name));
            resourceField.lock()->SetWidth(fPreviewSize);
            resourceField.lock()->SetHeight(fPreviewSize);
            resourceField.lock()->SetTooltip(EMakeRef<EUILabel>(std::to_string(res.ID)));
            resourceField.lock()->SetDragType("Resource" + fSelectedResourceType);
            resourceField.lock()->SetDragData({res.ID});
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
