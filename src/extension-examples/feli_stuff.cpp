#include "editor_extension.h"

using namespace Engine;

APP_ENTRY
{
    ERef<EUIPanel> markingPanel = EMakeRef<EUIPanel>("Text Marking");
    ERef<EUIResourceSelect> resourceSelect = EMakeRef<EUIResourceSelect>("Text");
    ERef<EUITextField> textLabel = EMakeRef<EUITextField>("Text");
    textLabel->SetMultiline(true);
    EWeakRef<EUITextField> weakTextLabel = textLabel;
    markingPanel->AddChild(resourceSelect);
    markingPanel->AddChild(textLabel);
    info.PanelRegister->RegisterItem(extensionName, markingPanel);

    resourceSelect->AddEventListener<events::EResourceSelectChangeEvent>([weakTextLabel](events::EResourceSelectChangeEvent event){
        ERef<EResource> resource = shared::GetResource(event.ResourceID);
        if (resource)
        {
            ETxtResource* rs = resource->GetCPtr<ETxtResource>();
            if (rs)
            {
                weakTextLabel.lock()->SetValue(rs->String);
            }
        }
    });
}


EXT_ENTRY
{
}