#include "editor.h"

using namespace Editor;
using namespace Graphics;
using namespace Engine;

E_STORAGE_STRUCT(MySubType, 
    (double, SomeDouble),
    (int, SomeMoreInt)
)

E_STORAGE_STRUCT(MyType, 
    (int, SomeInteger),
    (int, Other),
    (EString, SomeString),
    (EVector<MySubType>, Working)
)

EApplication::EApplication() 
    : fGraphicsContext(nullptr), fCommandLine(&EExtensionManager::instance().GetChaiContext())
{
    EExtensionManager::instance().AddEventListener<EExtensionLoadedEvent>([this](EExtensionLoadedEvent event) {
        EExtension* extension = EExtensionManager::instance().GetExtension(event.Extension);
        auto entry = (void(*)(const char*, Engine::EAppInit))extension->GetFunction("app_entry");
        if (entry)
        {
            EAppInit init;
            init.PanelRegister = &fUIRegister;
            entry(extension->GetName().c_str(), init);
        }
        auto initImGui = (void(*)())extension->GetFunction("InitImGui");
        if (initImGui)
        {
            initImGui();
        }
    });

    fUIRegister.AddEventListener<ERegisterChangedEvent>([this]() {
        this->RegenerateMainMenuBar();
    });
    EExtensionManager::instance().GetTypeRegister().RegisterItem("CORE", MyType::_dsc);
}

void EApplication::Start() 
{
    Wrapper::RunApplicationLoop(std::bind(&EApplication::Init, this, std::placeholders::_1), std::bind(&EApplication::Render, this), std::bind(&EApplication::RenderImGui, this), std::bind(&EApplication::CleanUp, this), &Wrapper::SetImGuiContext);
}

void EApplication::RegenerateMainMenuBar() 
{
    fMainMenu->Clear();

    ERef<EUIMenu> fileMenu = EMakeRef<EUIMenu>("File");
    ERef<EUIField> saveScene = fileMenu->AddChild(EMakeRef<EUIMenuItem>("Save"));
    saveScene->AddEventListener<events::EButtonEvent>([this](){
        EString saveToPath = Wrapper::SaveFileDialog("Save To", {"esc"});
        EJson json = ESerializer::WriteSceneToJson(EExtensionManager::instance().GetActiveScene());
        EFile file(saveToPath);
        file.SetFileAsString(json.dump());
    });
    ERef<EUIField> openScene = fileMenu->AddChild(EMakeRef<EUIMenuItem>("Open..."));
    openScene->AddEventListener<events::EButtonEvent>([this](){
        EVector<EString> openScene = Wrapper::OpenFileDialog("Open", {"esc"});
        if (openScene.size() > 0)
        {
            EFile sceneFile(openScene[0]);
            EJson sceneJson = EJson::parse(sceneFile.GetFileAsString());
            if (!sceneJson.is_null())
            {
                EDeserializer::ReadSceneFromJson(sceneJson, EExtensionManager::instance().GetActiveScene(), EExtensionManager::instance().GetTypeRegister().GetAllItems());
            }
        }
    });
    ERef<EUIField> importResource = fileMenu->AddChild(EMakeRef<EUIMenuItem>("Import..."));
    importResource->AddEventListener<events::EButtonEvent>([this](){
        EVector<EString> resourcesToOpen = Wrapper::OpenFileDialog("Import");
        for (const EString& resourcePath : resourcesToOpen)
        {
            EFile resourceFile(resourcePath);

            EString type = resourceFile.GetFileExtension();
            EResourceDescription foundDescription;
            if (EExtensionManager::instance().GetResourceRegister().FindItem(FindResourceByType(type), &foundDescription) &&
                foundDescription.ImportFunction)
            {
                EExtensionManager::instance().GetActiveScene()->GetResourceManager().ImportResourceFromFile(resourceFile, foundDescription);
            }
            else
            {
                // TODO: Show modal
                E_ERROR("Could not find converter for resource with file ending " + type);
            }

        }
    });


    ERef<EUIMenu> viewMenu = EMakeRef<EUIMenu>("View");
    for (EWeakRef<EUIPanel> panel : fUIRegister.GetAllItems())
    {
        EString panelLabel = panel.lock()->GetLabel();
        ERef<EUIMenuItem> menuItem = EMakeRef<EUIMenuItem>(panel.lock()->GetLabel());
        menuItem->AddEventListener<events::EButtonEvent>([panel](){
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
    fUIRegister.ClearAllItems();
}

void EApplication::Render() 
{
    
}

void EApplication::RenderImGui() 
{
    for (ERef<EUIPanel> panel : fUIRegister.GetAllItems())
    {
        panel->UpdateEventDispatcher();
    }

    for (ERef<EUIPanel> panel : fUIRegister.GetAllItems())
    {
        panel->Render();
    }

    fMainMenu->UpdateEventDispatcher();
    fMainMenu->Render();


    fCommandLine.UpdateEventDispatcher();
    fCommandLine.Render();
}

void EApplication::RegisterDefaultPanels() 
{
    ERef<EUIPanel> resourcePanel = EMakeRef<EUIPanel>("Resource Panel");
    resourcePanel->AddChild(EMakeRef<EResourceView>());



    ERef<EUIPanel> extensionPanel = EMakeRef<EUIPanel>("Extension Panel");
    ERef<EExtensionView> extensionView = EMakeRef<EExtensionView>();
    extensionPanel->AddChild(extensionView);


    ERef<EUIPanel> universalSceneView1 = EMakeRef<EUIPanel>("Basic Scene View 1");
    universalSceneView1->AddChild(EMakeRef<EObjectView>());
    ERef<EUIPanel> universalSceneView2 = EMakeRef<EUIPanel>("Basic Scene View 2");
    universalSceneView2->AddChild(EMakeRef<EObjectView>());
    ERef<EUIPanel> universalSceneView3 = EMakeRef<EUIPanel>("Basic Scene View 3");
    universalSceneView3->AddChild(EMakeRef<EObjectView>());
    ERef<EUIPanel> universalSceneView4 = EMakeRef<EUIPanel>("Basic Scene View 4");
    universalSceneView4->AddChild(EMakeRef<EObjectView>());

    fUIRegister.RegisterItem("Core", universalSceneView1);
    fUIRegister.RegisterItem("Core", universalSceneView2);
    fUIRegister.RegisterItem("Core", universalSceneView3);
    fUIRegister.RegisterItem("Core", universalSceneView4);
    fUIRegister.RegisterItem("Core", resourcePanel);
    fUIRegister.RegisterItem("Core", extensionPanel);
}