#include "editor.h"

#include "editor_icons_material_design.h"

using namespace Editor;
using namespace Graphics;
using namespace Engine;

E_STORAGE_STRUCT(MySubType, 
    (double, SomeDouble),
    (int, SomeMoreInt)
)

E_STORAGE_STRUCT(MyType, 
    (int, SomeInteger),
    (float, Other),
    (EString, SomeString),
    (EVector<MySubType>, Working)
)

EApplication::EApplication() 
    : fGraphicsContext(nullptr), fCommandLine(), fLoadOnStartRegister()
{
    shared::StaticSharedContext::instance().GetExtensionManager().AddEventListener<events::EExtensionLoadedEvent>([this](events::EExtensionLoadedEvent event) {
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

    shared::StaticSharedContext::instance().GetExtensionManager().AddEventListener<events::EExtensionUnloadEvent>([this](events::EExtensionUnloadEvent event){
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

void EApplication::Start(const EString& defaultRegisterPath) 
{
    fLoadOnStartRegister = defaultRegisterPath;
    Wrapper::RunApplicationLoop(std::bind(&EApplication::Init, this, std::placeholders::_1), std::bind(&EApplication::Render, this), std::bind(&EApplication::RenderImGui, this), std::bind(&EApplication::CleanUp, this), &Wrapper::SetImGuiContext);
}

void EApplication::RegenerateMainMenuBar() 
{
    fMainMenu->Clear();

    ERef<EUIMenu> fileMenu = EMakeRef<EUIMenu>("File");
    EWeakRef<EUIField> saveScene = fileMenu->AddChild(EMakeRef<EUIMenuItem>("Save"));
    saveScene.lock()->AddEventListener<events::EButtonEvent>([](){
        EString saveToPath = Wrapper::SaveFileDialog("Save To", {"esc"});
        if (!saveToPath.empty())
        {
            SaveRegisterToFile(saveToPath);
        }
    });
    EWeakRef<EUIField> openScene = fileMenu->AddChild(EMakeRef<EUIMenuItem>("Open..."));
    openScene.lock()->AddEventListener<events::EButtonEvent>([](){
        EVector<EString> openScene = Wrapper::OpenFileDialog("Open", {"esc"});
        if (openScene.size() > 0)
        {
            LoadRegisterFromFile(openScene[0]);
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

    // Setup connect callback
    auto connectFun = [textField, modal](){
        EString ip = std::dynamic_pointer_cast<EUITextField>(textField.lock())->GetContent();
        shared::StaticSharedContext::instance().ConnectTo(ip);
        modal->Close();
    };
    textField.lock()->AddEventListener<events::ETextCompleteEvent>(connectFun);
    connectButton.lock()->AddEventListener<events::EButtonEvent>(connectFun);


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

bool EApplication::LoadRegisterFromFile(const EString& path) 
{
    EFile sceneFile(path);
    if (!sceneFile.Exist()) { return false; }
    
    sceneFile.LoadToMemory();
    if (sceneFile.GetBuffer().IsNull()) { return false; }

    EFileCollection collection;
    collection.SetFromCompleteBuffer(sceneFile.GetBuffer());


    shared::StaticSharedContext::instance().GetExtensionManager().UnloadAll();
    bool foundRegister = false;
    for (auto& entry : collection)
    {
        if (entry.first == "Register.esc") { foundRegister = true; continue; }
        EFile saveExtension(entry.first);
        saveExtension.SetFileBuffer(entry.second);
        saveExtension.SaveBufferToDisk();
        shared::StaticSharedContext::instance().GetExtensionManager().LoadExtension(saveExtension.GetFullPath());
    }

    if (!foundRegister) { return false; }
    ESharedBuffer registerBuffer;
    if (collection.GetFileAt("Register.esc", &registerBuffer))
    {
        shared::LoadRegisterFromBuffer(registerBuffer);
        return true;
    }
    return false;
}

bool EApplication::SaveRegisterToFile(const EString& path) 
{
    EFile file(path);

    
    EFileCollection fileCollection;
    fileCollection.AddFile("Register.esc", shared::GetRegisterAsBuffer());

    for (EExtension* extension : shared::StaticSharedContext::instance().GetExtensionManager().GetLoadedExtensions())
    {
        EFile extensionFile(extension->GetFilePath());
        if (extensionFile.Exist())
        {
            extensionFile.LoadToMemory();
            fileCollection.AddFile(extension->GetName() + ".uex", extensionFile.GetBuffer());
        }
    }


    file.SetFileBuffer(fileCollection.GetCompleteBuffer());
    file.SaveBufferToDisk();
    return true;
}

void EApplication::Init(Graphics::GContext* context) 
{
    fGraphicsContext = context;
    fMainMenu = EMakeRef<EUIMainMenuBar>();
    RegisterDefaultPanels();



    RegenerateMainMenuBar();

    // For ImGui dll linkage
    Engine::intern::InitUI();


    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    
    EFile fontFile(FONT_ICON_FILE_NAME_MD);
    if (fontFile.Exist())
    {
        // merge in icons from Font Awesome
        static const ImWchar icons_ranges[] = { ICON_MIN_MD, ICON_MAX_MD, 0 };
        ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = false; icons_config.GlyphOffset = {0.0f, 4.0f};
        io.Fonts->AddFontFromFileTTF( FONT_ICON_FILE_NAME_MD, 16.0f, &icons_config, icons_ranges );
    }

    if (!fLoadOnStartRegister.empty())
    {
        LoadRegisterFromFile(fLoadOnStartRegister);
    }
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