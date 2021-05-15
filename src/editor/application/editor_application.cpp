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

void EApplication::RegenerateMainMenuBar() 
{
    fMainMenu->Clear();

    ERef<EUIMenu> fileMenu = EMakeRef<EUIMenu>("File");
    ERef<EUIField> saveScene = fileMenu->AddChild(EMakeRef<EUIMenuItem>("Save"));
    saveScene->AddEventListener<EClickEvent>([this](){
        EString saveToPath = Wrapper::SaveFileDialog("Save To", {"esc"});
        EJson json = ESerializer::WriteSceneToJson(fExtensionManager.GetActiveScene());
        EFile file(saveToPath);
        file.SetFileAsString(json.dump());
    });
    ERef<EUIField> openScene = fileMenu->AddChild(EMakeRef<EUIMenuItem>("Open"));
    openScene->AddEventListener<EClickEvent>([this](){
        EVector<EString> openScene = Wrapper::OpenFileDialog("Open", {"esc"});
        if (openScene.size() > 0)
        {
            EFile sceneFile(openScene[0]);
            EJson sceneJson = EJson::parse(sceneFile.GetFileAsString());
            if (!sceneJson.is_null())
            {
                EDeserializer::ReadSceneFromJson(sceneJson, fExtensionManager.GetActiveScene());
            }
        }
    });


    ERef<EUIMenu> viewMenu = EMakeRef<EUIMenu>("View");
    for (EWeakRef<EUIPanel> panel : fDefaultPanels)
    {
        EString panelLabel = panel.lock()->GetLabel();
        ERef<EUIMenuItem> menuItem = EMakeRef<EUIMenuItem>(panel.lock()->GetLabel());
        menuItem->AddEventListener<EClickEvent>([panel](){
            if (!panel.expired())
            {
                if (panel.lock()->IsOpen())
                {
                    ImGui::SetWindowFocus(panel.lock()->GetLabel().c_str());
                }
                else
                {
                    panel.lock()->Open();
                }
            }
        });
        viewMenu->AddChild(menuItem);
    }

    fMainMenu->AddChild(fileMenu);
    fMainMenu->AddChild(viewMenu);
}

void EApplication::Init(Graphics::GContext* context) 
{
    fGraphicsContext = context;
    fMainMenu = EMakeRef<EUIMainMenuBar>();
    RegisterDefaultPanels();

    // Load the extensions




    RegenerateMainMenuBar();

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

    fMainMenu->UpdateEventDispatcher();

    fMainMenu->Render();
}

void EApplication::RegisterDefaultPanels() 
{
    ERef<EUIPanel> resourcePanel = EMakeRef<EUIPanel>("Resource Panel");



    ERef<EUIPanel> extensionPanel = EMakeRef<EUIPanel>("Extension Panel");
    ERef<EExtensionView> extensionView = EMakeRef<EExtensionView>(&fExtensionManager);
    extensionPanel->AddChild(extensionView);


    ERef<EUIPanel> universalSceneView1 = EMakeRef<EUIPanel>("Basic Scene View 1");
    universalSceneView1->AddChild(EMakeRef<EObjectView>(fExtensionManager.GetActiveScene()));
    ERef<EUIPanel> universalSceneView2 = EMakeRef<EUIPanel>("Basic Scene View 2");
    universalSceneView2->AddChild(EMakeRef<EObjectView>(fExtensionManager.GetActiveScene()));
    ERef<EUIPanel> universalSceneView3 = EMakeRef<EUIPanel>("Basic Scene View 3");
    universalSceneView3->AddChild(EMakeRef<EObjectView>(fExtensionManager.GetActiveScene()));
    ERef<EUIPanel> universalSceneView4 = EMakeRef<EUIPanel>("Basic Scene View 4");
    universalSceneView4->AddChild(EMakeRef<EObjectView>(fExtensionManager.GetActiveScene()));

    fDefaultPanels.push_back(universalSceneView1);
    fDefaultPanels.push_back(universalSceneView2);
    fDefaultPanels.push_back(universalSceneView3);
    fDefaultPanels.push_back(universalSceneView4);
    fDefaultPanels.push_back(resourcePanel);
    fDefaultPanels.push_back(extensionPanel);
}