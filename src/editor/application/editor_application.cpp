#include "editor.h"

using namespace Editor;
using namespace Graphics;
using namespace Engine;

#define E_CREATE_STRUCT_PROP1(type, name) type name; 
#define E_CREATE_STRUCT_PROP(nametype) EXPAND( EXPAND ( E_CREATE_STRUCT_PROP1 nametype ) )



#define E_CREATE_STRUCT_DSC2(type, name) {E_STRINGIFY(name), ::Engine::getdsc::GetDescription<type>()},
#define E_CREATE_STRUCT_DSC(nametype) EXPAND( E_CREATE_STRUCT_DSC2 nametype )

#define E_GET_FROM_PROP2(type, name) ::Engine::EProperty* EXPAND ( E_CONCATENATE(prop, name) ) = property->GetProperty(E_STRINGIFY(name));
#define E_GET_FROM_PROP(nametype) EXPAND( E_GET_FROM_PROP2 nametype )

#define E_CHECK_NULL_AND2(type, name) EXPAND ( E_CONCATENATE(prop, name) ) &&
#define E_CHECK_NULL_AND(typename) EXPAND ( E_CHECK_NULL_AND2 typename )
#define E_CHECK_NULL_AND_LAST2(type, name) EXPAND ( E_CONCATENATE(prop, name) )
#define E_CHECK_NULL_AND_LAST(typename) EXPAND ( E_CHECK_NULL_AND_LAST2 typename )


#define E_SET_PROPERTY2(type, name) {::Engine::EValueDescription valDsc = ::Engine::getdsc::GetDescription<type>();\
                                    switch (valDsc.GetType())\
                                    {\
                                    case ::Engine::EValueType::STRUCT: static_cast<::Engine::EStructProperty*>(EXPAND ( E_CONCATENATE(prop, name) ) )->SetValue<type>(value. name );\
                                    case ::Engine::EValueType::PRIMITIVE: static_cast<::Engine::EValueProperty<type>*>(EXPAND ( E_CONCATENATE(prop, name) ) )->SetValue(value. name );\
                                    case ::Engine::EValueType::ENUM: break;/*TODO*/\
                                    case ::Engine::EValueType::UNKNOWN: break;\
                                    }}
#define E_SET_PROPERTY(typename) EXPAND ( E_SET_PROPERTY2 typename )


#define E_SET_SELF2(type, name) {::Engine::EValueDescription valDsc = ::Engine::getdsc::GetDescription<type>();\
                                    switch (valDsc.GetType())\
                                    {\
                                    case ::Engine::EValueType::STRUCT: static_cast<::Engine::EStructProperty*>(EXPAND ( E_CONCATENATE(prop, name) ) )->GetValue<type>(value. name );\
                                    case ::Engine::EValueType::PRIMITIVE: value. name = static_cast<::Engine::EValueProperty<type>*>(EXPAND ( E_CONCATENATE(prop, name) ) )->GetValue();\
                                    case ::Engine::EValueType::ENUM: break;/*TODO*/\
                                    case ::Engine::EValueType::UNKNOWN: break;\
                                    }}
#define E_SET_SELF(typename) EXPAND ( E_SET_SELF2 typename )


#define E_CHECK_EQUEL2(type, name) name == other. name &&
#define E_CHECK_EQUEL(typename) EXPAND (E_CHECK_EQUEL2 typename )

#define E_CHECK_EQUEL_LAST2(type, name) name == other. name
#define E_CHECK_EQUEL_LAST(typename) EXPAND (E_CHECK_EQUEL_LAST2 typename )

#define E_STORAGE_STRUCT(name, ...) struct name {\
                                        EXPAND (E_LOOP_ARGS(E_CREATE_STRUCT_PROP, __VA_ARGS__) )\
                                        static inline ::Engine::EValueDescription _dsc = ::Engine::EValueDescription::CreateStruct(EXPAND(E_STRINGIFY(name)), {\
                                            EXPAND (E_LOOP_ARGS(E_CREATE_STRUCT_DSC, __VA_ARGS__))\
                                        });\
                                        \
                                        static bool ToProperty(name & value, EStructProperty* property)\
                                        {\
                                            EXPAND( E_LOOP_ARGS(E_GET_FROM_PROP, __VA_ARGS__) ) \
                                            if (\
                                                EXPAND(E_LOOP_ARGS_L(E_CHECK_NULL_AND, __VA_ARGS__))\
                                            ){\
                                                EXPAND(E_LOOP_ARGS(E_SET_PROPERTY, __VA_ARGS__))\
                                                return true;\
                                            }\
                                            return false;\
                                        }\
                                        static bool FromProperty(name & value, EStructProperty* property)\
                                        {\
                                            EXPAND( E_LOOP_ARGS(E_GET_FROM_PROP, __VA_ARGS__) ) \
                                            if (\
                                                EXPAND(E_LOOP_ARGS_L(E_CHECK_NULL_AND, __VA_ARGS__))\
                                            ){\
                                                EXPAND(E_LOOP_ARGS(E_SET_SELF, __VA_ARGS__))\
                                            }\
                                            return false;\
                                        }\
                                        bool operator==(const name& other) const {\
                                            return \
                                            EXPAND(E_LOOP_ARGS_L(E_CHECK_EQUEL, __VA_ARGS__));\
                                        }\
                                        bool operator!=(const name& other) const { return !((*this) == other);}\
                                    };




#define E_STORAGE_TYPE(name, ...) EXPAND(E_STORAGE_STRUCT(name, __VA_ARGS__))

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
    /*fExtensionManager.AddEventListener<EExtensionLoadedEvent>([this](EExtensionLoadedEvent& event) {
        auto entry = (void(*)(const char*, Engine::EAppInit))event.Extension->GetFunction("app_entry");
        if (entry)
        {
            EAppInit init;
            init.PanelRegister = &fUIRegister;
            entry(event.Extension->GetName().c_str(), init);
        }
        auto initImGui = (void(*)())event.Extension->GetFunction("InitImGui");
        if (initImGui)
        {
            initImGui();
        }
    });*/

    /*fUIRegister.AddEventListener<ERegisterChangedEvent>([this]() {
        this->RegenerateMainMenuBar();
    });*/
    ERegister::Entity entity = fExtensionManager.GetActiveScene()->CreateEntity();
    fExtensionManager.GetActiveScene()->InsertComponent(entity, MyType::_dsc);
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