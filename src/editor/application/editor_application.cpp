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
    (EResourceLink, ImageLink, "Image"),
    (EVector<MySubType>, Working)
)

EApplication::EApplication() 
    : fGraphicsContext(nullptr), fCommandLine(), fLoadOnStartRegister()
{
    shared::ExtensionManager().AddEventListener<events::EExtensionLoadedEvent>([this](events::EExtensionLoadedEvent event) {
        EExtension* extension = shared::ExtensionManager().GetExtension(event.Extension);
        auto entry = (void(*)(const char*, Engine::EAppInit))extension->GetFunction("app_entry");
        if (entry)
        {
            EAppInit init;
            init.PanelRegister = &fUIRegister;
            init.ValueFieldRegister = &fUIValueRegister;
            E_INFO("Running APP_INIT for plugtin \"" + extension->GetName() + "\"");
            entry(extension->GetName().c_str(), init);
        }
        auto initImGui = (void(*)())extension->GetFunction("InitImGui");
        if (initImGui)
        {
            initImGui();
        }
    });

    shared::ExtensionManager().AddEventListener<events::EExtensionUnloadEvent>([this](events::EExtensionUnloadEvent event){
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
    });

    fUIRegister.AddEventListener<ERegisterChangedEvent>([this]() {
        this->RegenerateMainMenuBar();
    });
    shared::ExtensionManager().GetComponentRegister().RegisterStruct<MyType>("Core");
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
                    EResourceData* data = EResourceManager::CreateResourceFromFile(resourceFile, foundDescription);
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
    universalSceneView1->AddChild(EMakeRef<EObjectView>(&fUIValueRegister));
    ERef<EUIPanel> universalSceneView2 = EMakeRef<EUIPanel>("Basic Scene View 2");
    universalSceneView2->AddChild(EMakeRef<EObjectView>(&fUIValueRegister));
    ERef<EUIPanel> universalSceneView3 = EMakeRef<EUIPanel>("Basic Scene View 3");
    universalSceneView3->AddChild(EMakeRef<EObjectView>(&fUIValueRegister));
    ERef<EUIPanel> universalSceneView4 = EMakeRef<EUIPanel>("Basic Scene View 4");
    universalSceneView4->AddChild(EMakeRef<EObjectView>(&fUIValueRegister));

    fUIRegister.RegisterItem("Core", universalSceneView1);
    fUIRegister.RegisterItem("Core", universalSceneView2);
    fUIRegister.RegisterItem("Core", universalSceneView3);
    fUIRegister.RegisterItem("Core", universalSceneView4);
    fUIRegister.RegisterItem("Core", resourcePanel);
    fUIRegister.RegisterItem("Core", extensionPanel);
}

void EApplication::RegisterDefaultResources() 
{
    EResourceDescription imageDsc("Image",{"png", "jpeg", "bmp"});
    imageDsc.ImportFunction = &ResImage::ImportImage;
    shared::StaticSharedContext::instance().GetExtensionManager().GetResourceRegister().RegisterItem("Core", imageDsc);


    // FOR TESTING PURPOSES
    EResourceDescription pdfDescription("PDF", {"pdf"});
    shared::StaticSharedContext::instance().GetExtensionManager().GetResourceRegister().RegisterItem("Core", pdfDescription);

    EResourceDescription textDescription("Text", {"txt", "cpp"});
    shared::StaticSharedContext::instance().GetExtensionManager().GetResourceRegister().RegisterItem("Core", textDescription);
    
}

void EApplication::RegisterDefaultComponentRender() 
{
    fUIValueRegister.RegisterItem("Core", {EResourceLink::_dsc.GetId(), [](EProperty* prop, ERegister::Entity entity, const EString& nameIdent){
        ERef<EUIResourceSelect> resourceSelect = EMakeRef<EUIResourceSelect>("Image");
        resourceSelect->AddEventListener<events::EResourceSelectChangeEvent>([nameIdent, entity](events::EResourceSelectChangeEvent event){
            shared::SetValue<EResourceLink>(entity, nameIdent, EResourceLink("Image", event.ResourceID));
        });
        return resourceSelect;
    }});
}