#include "editor.h"

using namespace Editor;
using namespace Graphics;
using namespace Engine;


EApplication::EApplication() 
    : fGraphicsContext(nullptr)
{}

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
    for (ERef<EUIPanel> panel : fDefaultPanels)
    {
        panel->UpdateEventDispatcher();
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
    loadExtensionButton->AddEventListener<EClickEvent>([this](){
        EVector<EString> extensions = Wrapper::OpenFileDialog("Load Extension", {"uex"});
        for (const EString& extension : extensions)
        {
            this->fExtensionManager.LoadExtension(extension);
        }
    });
    extensionPanel->AddChild(loadExtensionButton);


    ERef<EUIPanel> universalSceneView = EMakeRef<EUIPanel>("Basic Scene View");
    universalSceneView->AddChild(EMakeRef<EObjectView>(fExtensionManager.GetActiveScene()));

    fDefaultPanels.push_back(universalSceneView);
    fDefaultPanels.push_back(resourcePanel);
    fDefaultPanels.push_back(extensionPanel);
}