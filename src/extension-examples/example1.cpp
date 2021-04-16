#include "engine_extension.h"



class TestUiField : public Engine::EUIField
{
public:
    TestUiField();

    virtual bool OnRender() override;
};

TestUiField::TestUiField() 
    : Engine::EUIField("UIField")
{
    
}

bool TestUiField::OnRender()
{
    ImGui::Button("SomeButton");
    return true;
}

EXT_ENTRY
{
    E_INFO(EString("Initiliazing ") + extensionName);
    ERef<Engine::EUIPanel> uiPanel = EMakeRef<Engine::EUIPanel>("First panel");
    uiPanel->AddChild(EMakeRef<TestUiField>());

    extension->UIRegister->RegisterItem(extensionName, uiPanel);
}