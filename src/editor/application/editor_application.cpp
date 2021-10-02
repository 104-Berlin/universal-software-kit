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
    : fGraphicsContext(nullptr), fCommandLine()
{
    shared::StaticSharedContext::instance().GetExtensionManager().AddEventListener<EExtensionLoadedEvent>([this](EExtensionLoadedEvent event) {
        EExtension* extension = shared::StaticSharedContext::instance().GetExtensionManager().GetExtension(event.Extension);
        auto entry = (void(*)(const char*, Engine::EAppInit))extension->GetFunction("app_entry");
        if (entry)
        {
            EAppInit init;
            init.PanelRegister = &fUIRegister;
            E_INFO("Running APP_INIT for plugtin \"" + extension->GetName() + "\"");
            entry(extension->GetName().c_str(), init);
        }
        auto initImGui = (void(*)())extension->GetFunction("InitImGui");
        if (initImGui)
        {
            initImGui();
        }
    });

    shared::StaticSharedContext::instance().GetExtensionManager().AddEventListener<EExtensionUnloadEvent>([this](EExtensionUnloadEvent event){
        for (ERef<EUIPanel> panel : fUIRegister.GetItems(event.ExtensionName))
        {
            panel->DisconnectAllEvents();
        }
        fUIRegister.ClearRegisteredItems(event.ExtensionName);
    });

    fUIRegister.AddEventListener<ERegisterChangedEvent>([this]() {
        this->RegenerateMainMenuBar();
    });
    shared::StaticSharedContext::instance().GetExtensionManager().GetTypeRegister().RegisterItem("CORE", MyType::_dsc);
}

void EApplication::Start() 
{
    Wrapper::RunApplicationLoop(std::bind(&EApplication::Init, this, std::placeholders::_1), std::bind(&EApplication::Render, this), std::bind(&EApplication::RenderImGui, this), std::bind(&EApplication::CleanUp, this), &Wrapper::SetImGuiContext);
}

void EApplication::RegenerateMainMenuBar() 
{
    fMainMenu->Clear();

    ERef<EUIMenu> fileMenu = EMakeRef<EUIMenu>("File");
    EWeakRef<EUIField> saveScene = fileMenu->AddChild(EMakeRef<EUIMenuItem>("Save"));
    saveScene.lock()->AddEventListener<events::EButtonEvent>([this](){
        EString saveToPath = Wrapper::SaveFileDialog("Save To", {"esc"});
        if (!saveToPath.empty())
        {
            EFile file(saveToPath);
            //file.SetFileBuffer(ESerializer::WriteFullSceneBuffer(EExtensionManager::instance().GetActiveScene()));
            //file.SaveBufferToDisk();
        }
    });
    EWeakRef<EUIField> openScene = fileMenu->AddChild(EMakeRef<EUIMenuItem>("Open..."));
    openScene.lock()->AddEventListener<events::EButtonEvent>([this](){
        EVector<EString> openScene = Wrapper::OpenFileDialog("Open", {"esc"});
        if (openScene.size() > 0)
        {
            EFile sceneFile(openScene[0]);
            sceneFile.LoadToMemory();
            if (!sceneFile.GetBuffer().IsNull())
            {
                shared::StaticSharedContext::instance().GetExtensionManager().Reload();
                ///EDeserializer::ReadSceneFromFileBuffer(sceneFile.GetBuffer(), EExtensionManager::instance().GetActiveScene(), EExtensionManager::instance().GetTypeRegister().GetAllItems());
            }
        }
    });
    EWeakRef<EUIField> importResource = fileMenu->AddChild(EMakeRef<EUIMenuItem>("Import..."));
    importResource.lock()->AddEventListener<events::EButtonEvent>([this](){
        EVector<EString> resourcesToOpen = Wrapper::OpenFileDialog("Import");
        for (const EString& resourcePath : resourcesToOpen)
        {
            EFile resourceFile(resourcePath);

            EString type = resourceFile.GetFileExtension();
            EResourceDescription foundDescription;
            if (shared::StaticSharedContext::instance().GetExtensionManager().GetResourceRegister().FindItem(EFindResourceByType(type), &foundDescription) &&
                foundDescription.ImportFunction)
            {
                EResourceData* data = EResourceManager::CreateResourceFromFile(resourceFile, foundDescription);
                if (data)
                {
                    shared::CreateResource(data);
                    delete data;
                }
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

    ERef<EUIMenu> connectionMenu = EMakeRef<EUIMenu>("Connection");
    ERef<EUIModal> modal = EMakeRef<EUIModal>("Connect Modal");
    EWeakRef<EUIField> textField = modal->AddChild(EMakeRef<EUITextField>("IP Address"));
    EWeakRef<EUIField> connectButton = modal->AddChild(EMakeRef<EUIButton>("Connect"));
    connectButton.lock()->AddEventListener<events::EButtonEvent>([textField, modal](){
        EString ip = std::dynamic_pointer_cast<EUITextField>(textField.lock())->GetContent();
        shared::StaticSharedContext::instance().ConnectTo(ip);
        modal->Close();
    });
    ERef<EUIMenuItem> connectToItem = EMakeRef<EUIMenuItem>("Connect To...");
    connectToItem->AddEventListener<events::EButtonEvent>([modal](){
        modal->Open();
    });
    connectionMenu->AddChild(connectToItem);

    fMainMenu->AddChild(fileMenu);
    fMainMenu->AddChild(viewMenu);
    fMainMenu->AddChild(connectionMenu);

    fMainMenu->AddChild(modal);
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

    ImGui::ShowDemoWindow();

    shared::StaticSharedContext::instance().GetRegisterConnection().GetEventDispatcher().Update();
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