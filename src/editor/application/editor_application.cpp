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
    (float, Other),
    (EString, SomeString),
    (EResourceLink, ImageLink, "Image"),
    (EResourceLink, MeshLink, "Mesh"),
    (EVector<MySubType>, Working)
)

E_STORAGE_ENUM(MyEnum,
    One,
    Two,
    Three
)

E_STORAGE_STRUCT(EnumTest,
    (MyEnum, SomeEnum)
)

static EApplication* runningInstance = nullptr;

EApplication::EApplication() 
    : fGraphicsContext(nullptr), fUIRegister(), fUIValueRegister(), fViewportRenderFunctionRegister(), fViewportManager(&fUIRegister, &fViewportRenderFunctionRegister), fCommandLine(), fLoadOnStartRegister()
{
    EFolder folder(EFile::GetAppDataPath());
    if (!folder.Exist())
    {
        folder.Create();
    }

    shared::ExtensionManager().AddEventListener<events::EExtensionLoadedEvent>([this](events::EExtensionLoadedEvent event) {
        EExtension* extension = shared::ExtensionManager().GetExtension(event.Extension);
        auto entry = (void(*)(const char*, Engine::EAppInit))extension->GetFunction("app_entry");
        if (entry)
        {
            EAppInit init;
            init.PanelRegister = &fUIRegister;
            init.ValueFieldRegister = &fUIValueRegister;
            init.ViewportRenderFunctions = &fViewportRenderFunctionRegister;
            E_INFO("Running APP_INIT for plugtin \"" + extension->GetName() + "\"");
            entry(extension->GetName().c_str(), init);
        }
        auto initImGui = (void(*)())extension->GetFunction("InitImGui");
        if (initImGui)
        {
            initImGui();
        }

        fViewportManager.ReloadViewports();
    });


    shared::Events().Connect<events::EExtensionUnloadEvent>([this](events::EExtensionUnloadEvent event){
        for (ERef<EUIPanel> panel : fUIRegister.GetItems(event.ExtensionName))
        {
            panel->DisconnectAllEvents();
        }
        EExtension* extension = shared::ExtensionManager().GetExtension(event.ExtensionName);
        auto entry = (void(*)())extension->GetFunction("app_cleanup");
        if (entry)
        {
            entry();
        }

        fUIRegister.ClearRegisteredItems(event.ExtensionName);
        fUIValueRegister.ClearRegisteredItems(event.ExtensionName);
        fViewportRenderFunctionRegister.ClearRegisteredItems(event.ExtensionName);

        fViewportManager.ReloadViewports();
    });

    fUIRegister.AddEventListener<ERegisterChangedEvent>([this]() {
        this->RegenerateMainMenuBar();
    });

    // Temp
    shared::ExtensionManager().GetComponentRegister().RegisterStruct<MyType>("Core");
    shared::ExtensionManager().GetComponentRegister().RegisterStruct<EnumTest>("Core");
}

EApplication::~EApplication() 
{
    SaveApplicationState();
    runningInstance = nullptr;
}

void EApplication::Start(const EString& defaultRegisterPath) 
{
    if (runningInstance)
    {
        E_ERROR("Application already started");
        return;
    }
    E_INFO(EFile::GetAppDataPath());
    runningInstance = this;
    fLoadOnStartRegister = defaultRegisterPath;
    Wrapper::RunApplicationLoop(std::bind(&EApplication::Init, this, std::placeholders::_1), std::bind(&EApplication::Render, this), std::bind(&EApplication::RenderImGui, this), std::bind(&EApplication::CleanUp, this), &Wrapper::SetImGuiContext);
}

void EApplication::RegenerateMainMenuBar() 
{
    fMainMenu->Clear();

    ERef<EUIMenu> fileMenu = EMakeRef<EUIMenu>("File");
    EWeakRef<EUIField> saveScene = fileMenu->AddChild(EMakeRef<EUIMenuItem>("Save" ICON_MD_ALARM));
    saveScene.lock()->AddEventListener<events::EButtonEvent>([this](){
        EString saveToPath = Wrapper::SaveFileDialog("Save To", {"esc"});
        if (!saveToPath.empty())
        {
            EFile file(saveToPath);
            file.SetFileBuffer(shared::GetRegisterAsBuffer());
            file.SaveBufferToDisk();
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
                shared::ExtensionManager().Reload();
                shared::LoadRegisterFromBuffer(sceneFile.GetBuffer());
            }
        }
    });
    EWeakRef<EUIField> importResource = fileMenu->AddChild(EMakeRef<EUIMenu>("Import"));
    for (const EResourceDescription& resourceType : shared::ExtensionManager().GetResourceRegister().GetAllItems())
    {
        EWeakRef<EUIField> importItem = importResource.lock()->AddChild(EMakeRef<EUIMenuItem>(resourceType.ResourceName));
        importItem.lock()->AddEventListener<events::EButtonEvent>([this, resourceType](){
            EVector<EString> resourcesToOpen = Wrapper::OpenFileDialog("Import", resourceType.AcceptedFileEndings);
            for (const EString& resourcePath : resourcesToOpen)
            {
                EFile resourceFile(resourcePath);

                EString type = resourceFile.GetFileExtension();
                EResourceDescription foundDescription;
                if (shared::ExtensionManager().GetResourceRegister().FindItem(EFindResourceByType(type), &foundDescription) &&
                    foundDescription.ImportFunction)
                {
                    EResource* data = EResourceManager::CreateResourceFromFile(resourceFile, foundDescription);
                    if (data)
                    {
                        shared::CreateResource(data);
                        delete data;
                    }
                }
                else
                {
                }

            }
        });
    }


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

void EApplication::Init(Graphics::GContext* context) 
{
    fGraphicsContext = context;

    fMainMenu = EMakeRef<EUIMainMenuBar>();
    RegisterDefaultPanels();
    RegisterDefaultResources();
    RegisterDefaultComponentRender();



    RegenerateMainMenuBar();

    // For ImGui dll linkage
    Engine::intern::InitUI();


    ImGuiIO& io = ImGui::GetIO();
    
#include "MaterialIcons-Regular.h"
#include "Roboto-Regular.h"
    ImFontConfig icons_config_01; icons_config_01.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromMemoryTTF((void*)Roboto_Regular_buffer, Roboto_Regular_size, 16.0f, &icons_config_01);

    // merge in icons from Font Awesome
    static const ImWchar icons_ranges[] = { ICON_MIN_MD, ICON_MAX_MD, 0 };
    ImFontConfig icons_config; icons_config.FontDataOwnedByAtlas = false; icons_config.MergeMode = true; icons_config.PixelSnapH = false; icons_config.GlyphOffset = {0.0f, 4.0f};
    io.Fonts->AddFontFromMemoryTTF((void*)MaterialIcons_Regular_buffer, MaterialIcons_Regular_size, 16.0f, &icons_config, icons_ranges);

    LoadApplicationState();

    if (!fLoadOnStartRegister.empty())
    {
        EFile registerFile(fLoadOnStartRegister);
        if (registerFile.Exist())
        {
            registerFile.LoadToMemory();
            shared::LoadRegisterFromBuffer(registerFile.GetBuffer());
        }
    }
}

void EApplication::CleanUp() 
{
    fUIRegister.ClearAllItems();
}

void EApplication::Render() 
{        
    return;
    EVec2 mousePos(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
    

    ImVec2 md0 = ImGui::GetMouseDragDelta(0, 0.0f);
    EVec2 mouseDrag0(md0.x, md0.y);
    ImGui::ResetMouseDragDelta(0);
    if (glm::length(mouseDrag0) > 0.0f)
    {
        events::EMouseDragEvent dragEvent;
        dragEvent.MouseButton = 0;
        dragEvent.MouseDelta = mouseDrag0;
        dragEvent.Position = mousePos;
        //fCameraControls->OnMouseDrag(dragEvent);
    }

    float scrollX = ImGui::GetIO().MouseWheel;
    float scrollY = ImGui::GetIO().MouseWheelH;

    if (scrollX != 0.0f || scrollY != 0.0f)
    {
        events::EMouseScrollEvent scrollEvent;
        scrollEvent.ScrollX = scrollX;
        scrollEvent.ScrollY = scrollY;
        //fCameraControls->OnMouseScroll(scrollEvent);
    }

    fGraphicsContext->EnableDepthTest(true);
    fGraphicsContext->SetFaceCullingMode(Graphics::GCullMode::NONE);
    
}

void EApplication::RenderImGui() 
{
    //E_INFO("Active ID: " + std::to_string(ImGui::GetActiveID()));
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

    ERef<EUIPanel> connectionStatePanel = EMakeRef<EUIPanel>("Connection Panel");
    connectionStatePanel->AddChild(EMakeRef<EServerStatusUI>());


    ERef<EUIPanel> universalSceneView1 = EMakeRef<EUIPanel>("Basic Scene View 1");
    universalSceneView1->AddChild(EMakeRef<EObjectView>(&fUIValueRegister));
    ERef<EUIPanel> universalSceneView2 = EMakeRef<EUIPanel>("Basic Scene View 2");
    universalSceneView2->AddChild(EMakeRef<EObjectView>(&fUIValueRegister));
    ERef<EUIPanel> universalSceneView3 = EMakeRef<EUIPanel>("Basic Scene View 3");
    universalSceneView3->AddChild(EMakeRef<EObjectView>(&fUIValueRegister));
    ERef<EUIPanel> universalSceneView4 = EMakeRef<EUIPanel>("Basic Scene View 4");
    universalSceneView4->AddChild(EMakeRef<EObjectView>(&fUIValueRegister));


    ERef<EUIPanel> registerPanel = EMakeRef<EUIPanel>("Register Panel");
    registerPanel->AddChild(EMakeRef<EBasicRegisterView>());

    fUIRegister.RegisterItem("Core", universalSceneView1);
    //fUIRegister.RegisterItem("Core", universalSceneView2);
    //fUIRegister.RegisterItem("Core", universalSceneView3);
    //fUIRegister.RegisterItem("Core", universalSceneView4);
    fUIRegister.RegisterItem("Core", resourcePanel);
    fUIRegister.RegisterItem("Core", extensionPanel);
    fUIRegister.RegisterItem("Core", connectionStatePanel);
    fUIRegister.RegisterItem("Core", registerPanel);
}

void EApplication::RegisterDefaultResources() 
{
    EResourceDescription imageDsc("Image",{"png", "jpeg", "bmp"});
    imageDsc.ImportFunction = [](ESharedBuffer buffer) -> EResource*{
        EResource* result = new EResource("Image");
        result->SetBuffer(buffer);
        result->Load<EImageResource>();
        return result;
    };

    EResourceDescription meshDsc("Mesh",{"obj", "fbx", "dae", "3ds", "glb", "gltf", "blend"});
    meshDsc.ImportFunction = [](ESharedBuffer buffer) -> EResource*{
        EResource* result = new EResource("Mesh");
        result->SetBuffer(buffer);
        result->Load<EMeshResource>();
        return result;
    };

    shared::StaticSharedContext::instance().GetExtensionManager().GetResourceRegister().RegisterItem("Core", imageDsc);
    shared::StaticSharedContext::instance().GetExtensionManager().GetResourceRegister().RegisterItem("Core", meshDsc);


    // FOR TESTING PURPOSES
    EResourceDescription pdfDescription("PDF", {"pdf"});
    shared::StaticSharedContext::instance().GetExtensionManager().GetResourceRegister().RegisterItem("Core", pdfDescription);

    EResourceDescription textDescription("Text", {"txt", "cpp"});
    shared::StaticSharedContext::instance().GetExtensionManager().GetResourceRegister().RegisterItem("Core", textDescription);
    
}

void EApplication::RegisterDefaultComponentRender() 
{
    fUIValueRegister.RegisterItem("Core", {EResourceLink::_dsc.GetId(), [](EProperty* prop, const EString& nameIdent, std::function<void(EProperty*)> callbackFn)->ERef<EUIField>{
        EResourceLink link;
        if (convert::getter(prop, &link))
        {
            ERef<EUIResourceSelect> resourceSelect = EMakeRef<EUIResourceSelect>(link.Type);
            resourceSelect->AddEventListener<events::EResourceSelectChangeEvent>([nameIdent, callbackFn, link](events::EResourceSelectChangeEvent event){
                EResourceLink newLink;
                newLink.Type = event.ResourceType;
                newLink.ResourceId = event.ResourceID;

                ERef<EStructProperty> structProp = std::dynamic_pointer_cast<EStructProperty>(EProperty::CreateFromDescription(EResourceLink::_dsc.GetId(), EResourceLink::_dsc));
                if (convert::setter(structProp.get(), newLink))
                {
                    callbackFn(structProp.get());
                }
            });
            resourceSelect->SetResourceLink(link);
            return resourceSelect;
        }
        return EMakeRef<EUIField>("ResourceLink");
    }});
}

ApplicationState EApplication::CreateApplicationState() const
{
    ApplicationState result;

    for (EExtension* ext : shared::ExtensionManager().GetLoadedExtensions())
    {
        if (ext->GetAutoLoad())
        {
            result.AutoLoadExtensions.push_back(ext->GetFilePath());
        }
    }

    return result;
}

void EApplication::SaveApplicationState() const
{
    ERef<EProperty> appStateProp = EProperty::CreateFromDescription(ApplicationState::_dsc.GetId(), ApplicationState::_dsc);
    convert::setter(appStateProp.get(), CreateApplicationState());

    EString appStateJsonString = ESerializer::WritePropertyToJs(appStateProp.get(), false).dump();

    EFile stateFile(Path::Join(EFile::GetAppDataPath(), "application_state.uas"));
    stateFile.SetFileAsString(appStateJsonString);
    E_INFO("Saving application state to: " + stateFile.GetFullPath());
}

void EApplication::LoadApplicationState()
{
    EFile stateFile(Path::Join(EFile::GetAppDataPath(), "application_state.uas"));
    if (!stateFile.Exist())
    {
        E_WARN("Application state file not found");
        return;
    }

    EString stateString = stateFile.GetFileAsString();
    EJson appStateJson = EJson::parse(stateString);

    ERef<EProperty> appStateProp = EProperty::CreateFromDescription(ApplicationState::_dsc.GetId(), ApplicationState::_dsc);
    EDeserializer::ReadPropertyFromJson(appStateJson, appStateProp.get());


    ApplicationState state;
    convert::getter(appStateProp.get(), &state);

    for (const EString& autoLoadExtension : state.AutoLoadExtensions)
    {
        EFile file(autoLoadExtension);
        shared::ExtensionManager().SetExtensionAutoLoad(file.GetFileName(), true);
        shared::ExtensionManager().LoadExtension(autoLoadExtension);
    }
}

EApplication* EApplication::gApp() 
{
    E_ASSERT_M(runningInstance != nullptr, "Application not initialized");
    return runningInstance;
}