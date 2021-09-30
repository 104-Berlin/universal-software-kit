#include "editor_extension.h"

using namespace Engine;
using namespace Graphics;
using namespace Renderer;

APP_ENTRY
{
    ERef<EUIPanel> showPanel = EMakeRef<EUIPanel>("Show Panel");
    ERef<EUITextField> textField = EMakeRef<EUITextField>("Name");
    textField->AddEventListener<events::ETextCompleteEvent>([](const events::ETextCompleteEvent& evt){
        E_INFO("Text changed: " + evt.Value);
    });
    ERef<EUIButton> button = EMakeRef<EUIButton>("Click Me");
    button->AddEventListener<events::EButtonEvent>([](){
        E_INFO("BUTTON CLICK");
    });
    button->AddEventListener<events::EMouseDownEvent>([](){
        E_INFO("MOUSE CLICK");
    });
    ERef<EUIFloatEdit> floatField = EMakeRef<EUIFloatEdit>("Float");
    
    showPanel->AddChild(textField);
    showPanel->AddChild(button);
    showPanel->AddChild(floatField);



    info.PanelRegister->RegisterItem(extensionName, showPanel);
}