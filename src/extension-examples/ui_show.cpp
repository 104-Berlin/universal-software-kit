#include "editor_extension.h"

using namespace Engine;
using namespace Graphics;
using namespace Renderer;

APP_ENTRY
{
    ERef<EUIPanel> showPanel = EMakeRef<EUIPanel>("Show Panel");
    ERef<EUITextField> textField = EMakeRef<EUITextField>("Name");
    textField->AddEventListener<events::ETextChangeEvent>([](const events::ETextChangeEvent& evt){
        E_INFO("Text changed: " + evt.Value);
    });
    ERef<EUIButton> button = EMakeRef<EUIButton>("Click Me");
    button->AddEventListener<events::EButtonEvent>([](){
        E_INFO("BUTTON CLICK");
    });
    button->AddEventListener<events::EMouseDownEvent>([](){
        E_INFO("MOUSE CLICK");
    });
    showPanel->AddChild(textField);
    showPanel->AddChild(button);



    info.PanelRegister->RegisterItem(extensionName, showPanel);
}