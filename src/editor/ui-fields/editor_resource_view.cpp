#include "editor.h"

using namespace Editor;
using namespace Engine;

EResourceView::EResourceView() 
    : EUIField("RESOURCE_MANAGER"), selectedResource(0)
{

    EWeakRef<EUIField> resourceTypeContainer = AddChild(EMakeRef<EUIContainer>());
    resourceTypeContainer.lock()->SetWidth(150);
    resourceTypeContainer.lock()->SetDirty();
    

    resourceTypeContainer.lock()->SetCustomUpdateFunction([resourceTypeContainer](){
        if (resourceTypeContainer.expired()) { return; }
        resourceTypeContainer.lock()->Clear();
        auto allResources = shared::ExtensionManager().GetResourceRegister().GetAllItems();
        for (auto res : allResources)
        {
            EWeakRef<EUIField> selectField = resourceTypeContainer.lock()->AddChild(EMakeRef<EUISelectable>(res.ResourceName));
            selectField.lock()->AddEventListener<events::ESelectionChangeEvent>([](events::ESelectionChangeEvent event){
                if (event.Selected)
                {

                }
                else
                {

                }
            });
        }
    });

    shared::Events().Connect<events::EExtensionLoadedEvent>([this](events::EExtensionLoadedEvent e){
        RegenAddComponentMenu();
    }, this);
    shared::Events().Connect<events::EResourceAddedEvent>([](events::EResourceAddedEvent event){
        
    });
}
