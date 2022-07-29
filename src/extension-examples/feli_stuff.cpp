#include "editor_extension.h"

using namespace Engine;

APP_ENTRY
{
    ERef<EUIPanel> markingPanel = EMakeRef<EUIPanel>("Text Marking");
    ERef<EUIResourceSelect> resourceSelect = EMakeRef<EUIResourceSelect>("Text");
    ERef<EUILabel> textLabel = EMakeRef<EUILabel>();
    EWeakRef<EUILabel> weakTextLabel = textLabel;
    markingPanel->AddChild(resourceSelect);
    markingPanel->AddChild(textLabel);
    info.PanelRegister->RegisterItem(extensionName, markingPanel);

    resourceSelect->AddEventListener<events::EResourceSelectChangeEvent>([weakTextLabel](events::EResourceSelectChangeEvent event){

    });
}


EXT_ENTRY
{
}