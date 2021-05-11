#include "editor.h"

using namespace Editor;
using namespace Graphics;
using namespace Engine;

static EComponentDescription testCompDsc("TestComponent", {
    {EValueType::BOOL, "MyBoolean"},
    {EValueType::INTEGER, "MyInteger"},
    {EValueType::STRING, "SomeString"}
});

EApplication::EApplication() 
    : fGraphicsContext(nullptr)
{
    fScenePointer = new Engine::EScene("Unknown Scene 1");
    // TESTING CODE
    fScenePointer->RegisterComponent(testCompDsc);
}

void EApplication::Start() 
{
    Wrapper::RunApplicationLoop(std::bind(&EApplication::Init, this, std::placeholders::_1), std::bind(&EApplication::Render, this), std::bind(&EApplication::RenderImGui, this), std::bind(&EApplication::CleanUp, this), &Wrapper::SetImGuiContext);
}

void EApplication::Init(Graphics::GContext* context) 
{
    fGraphicsContext = context;
    RegisterDefaultPanels();

    // Load the extensions




    // For ImGui dll linkage
    Engine::intern::InitUI();
}

void EApplication::CleanUp() 
{
    
}

void EApplication::Render() 
{
    
}

void EApplication::RenderImGui() 
{
    EVector<ERef<EUIPanel>> allPanels = fExtensionManager.GetRegisteres().UIRegister->GetAllItems();
    // Fire off all events
    for (ERef<EUIPanel> panel : allPanels)
    {
        panel->UpdateEventDispatcher();
    }

    for (ERef<EUIPanel> panel : fDefaultPanels)
    {
        panel->UpdateEventDispatcher();
    }


    // Render them after words
    for (ERef<EUIPanel> panel : allPanels)
    {
        panel->Render();
    }

    for (ERef<EUIPanel> panel : fDefaultPanels)
    {
        panel->Render();
    }
}

void EApplication::RegisterDefaultPanels() 
{
    ERef<EUIPanel> resourcePanel = EMakeRef<EUIPanel>("Resource Panel");



    ERef<EUIPanel> extensionPanel = EMakeRef<EUIPanel>("Extension Panel");
    ERef<EUIField> loadExtensionButton = EMakeRef<EUIButton>("Load Extension");
    loadExtensionButton->AddEventListener<EClickEvent>([](){
        E_INFO("Clicking button");
    });
    extensionPanel->AddChild(loadExtensionButton);


    ERef<EUIPanel> universalSceneView = EMakeRef<EUIPanel>("Basic Scene View");
    universalSceneView->AddChild(EMakeRef<EObjectView>(fScenePointer));

    fDefaultPanels.push_back(universalSceneView);
    fDefaultPanels.push_back(resourcePanel);
    fDefaultPanels.push_back(extensionPanel);
}