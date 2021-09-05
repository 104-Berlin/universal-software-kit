#include "editor.h"

using namespace Editor;
using namespace Graphics;
using namespace Engine;

E_STORAGE_TYPE(MySubType, 
    (double, SomeDouble),
    (int, SomeMoreInt)
)

E_STORAGE_TYPE(MyType, 
    (int, SomeInteger),
    (int, Other),
    (EString, SomeString),
    (EVector<MySubType>, Working)
)

EApplication::EApplication() 
    : fGraphicsContext(nullptr), fCommandLine(&fExtensionManager.GetChaiContext())
{
    fExtensionManager.AddEventListener<EExtensionLoadedEvent>([this](EExtensionLoadedEvent event) {
        EExtension* extension = fExtensionManager.GetExtension(event.Extension);
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
    fExtensionManager.GetTypeRegister().RegisterItem("CORE", MyType::_dsc);

    std::cout << "VECTOR?" << is_vector<EString>::value << std::endl;
    std::cout << getdsc::GetDescription<EString>().IsArray() << std::endl;
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
        EJson json = ESerializer::WriteSceneToJson(fExtensionManager.GetActiveScene());
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
                EDeserializer::ReadSceneFromJson(sceneJson, fExtensionManager.GetActiveScene(), fExtensionManager.GetTypeRegister().GetAllItems());
            }
        }
    });
    ERef<EUIField> importResource = fileMenu->AddChild(EMakeRef<EUIMenuItem>("Import..."));
    importResource->AddEventListener<events::EButtonEvent>([this](){
        EVector<EString> resourcesToOpen = Wrapper::OpenFileDialog("Import");
        for (const EString& resourcePath : resourcesToOpen)
        {
            EFile resourceFile(resourcePath);

            resourceFile.LoadToMemory();
            ESharedBuffer fileBuffer = resourceFile.GetBuffer();
            
            byte* data = (byte*) malloc(fileBuffer.GetSizeInByte());
            memcpy(data, fileBuffer.Data(), fileBuffer.GetSizeInByte());
            fExtensionManager.GetActiveScene()->GetResourceManager().RegisterResource(resourceFile.GetFileExtension(), resourcePath,data, fileBuffer.GetSizeInByte());
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
    resourcePanel->AddChild(EMakeRef<EResourceView>(&fExtensionManager.GetActiveScene()->GetResourceManager()));



    ERef<EUIPanel> extensionPanel = EMakeRef<EUIPanel>("Extension Panel");
    ERef<EExtensionView> extensionView = EMakeRef<EExtensionView>(&fExtensionManager);
    extensionPanel->AddChild(extensionView);


    ERef<EUIPanel> universalSceneView1 = EMakeRef<EUIPanel>("Basic Scene View 1");
    universalSceneView1->AddChild(EMakeRef<EObjectView>(&fExtensionManager));
    ERef<EUIPanel> universalSceneView2 = EMakeRef<EUIPanel>("Basic Scene View 2");
    universalSceneView2->AddChild(EMakeRef<EObjectView>(&fExtensionManager));
    ERef<EUIPanel> universalSceneView3 = EMakeRef<EUIPanel>("Basic Scene View 3");
    universalSceneView3->AddChild(EMakeRef<EObjectView>(&fExtensionManager));
    ERef<EUIPanel> universalSceneView4 = EMakeRef<EUIPanel>("Basic Scene View 4");
    universalSceneView4->AddChild(EMakeRef<EObjectView>(&fExtensionManager));

    fUIRegister.RegisterItem("Core", universalSceneView1);
    fUIRegister.RegisterItem("Core", universalSceneView2);
    fUIRegister.RegisterItem("Core", universalSceneView3);
    fUIRegister.RegisterItem("Core", universalSceneView4);
    fUIRegister.RegisterItem("Core", resourcePanel);
    fUIRegister.RegisterItem("Core", extensionPanel);
}