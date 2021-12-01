#include "prefix_interface.h"

// Some helpers for quicker function calls
#define EXTENSION_MANAGER StaticSharedContext::instance().GetExtensionManager()
#define ACTIVE_SCENE StaticSharedContext::instance().GetExtensionManager().GetActiveScene()

using namespace Engine;

shared::StaticSharedContext* shared::StaticSharedContext::fInstance = nullptr;


shared::ERegisterEventDispatcher& shared::Events()
{
    return shared::StaticSharedContext::instance().Events();
}

EExtensionManager& shared::ExtensionManager()
{
    return shared::StaticSharedContext::instance().GetExtensionManager();
}

shared::ESharedError shared::LoadExtension(const EString& pathToExtension) 
{
    
    /*StaticSharedContext::instance().RunInMainThread([pathToExtension](){
        EFile file(pathToExtension);
        if (!file.Exist())
        {
            E_ERROR("Could not find path to extension! " + pathToExtension);
        }
        if (!EXTENSION_MANAGER.LoadExtension(pathToExtension))
        {
            return; // ERROR
        }
        EExtension* extension = EXTENSION_MANAGER.GetExtension(file.GetFileName());
        E_ASSERT(extension);
        E_INFO("Loaded extension \"" + extension->GetName() + "\"");
    });*/

    return false;
}

shared::ESharedError shared::CreateEntity() 
{
    StaticSharedContext::instance().GetRegisterConnection().Send_CreateNewEntity();
    return false;
}

shared::ESharedError shared::LoadRegisterFromBuffer(ESharedBuffer buffer)
{
    if (buffer.IsNull())
    {
        E_ERROR("Cant load register from empty buffer!");
        return true;
    }
    StaticSharedContext::instance().GetRegisterConnection().Send_LoadRegister(buffer);
    return false;
}


shared::ESharedError shared::CreateComponent(const EString& componentId, EDataBase::Entity entity) 
{
    EComponentRegisterEntry desc;
    if (!EXTENSION_MANAGER.GetComponentRegister().FindItem(EFindTypeDescByName(componentId), &desc))
    {
        E_ERROR("Could not find type" + componentId);
        return true; // ERROR
    }
    if (desc.DefaultValue)
    {
        CreateComponent(desc.DefaultValue.get(), entity);
    }
    else
    {
        CreateComponent(desc.Description, entity);
    }    
    
    return false;
}

shared::ESharedError shared::CreateComponent(const EValueDescription& componentId, EDataBase::Entity entity) 
{
    if (componentId.GetType() != EValueType::STRUCT)
    {
        E_ERROR("You have to add struct as component");
        return true;
    }
    StaticSharedContext::instance().GetRegisterConnection().Send_CreateNewComponent(entity, static_cast<EStructProperty*>(EProperty::CreateFromDescription(componentId.GetId(), componentId)));
    return false;
}

shared::ESharedError shared::CreateComponent(EStructProperty* componentValue, EDataBase::Entity entity)
{
    if (!componentValue)
    {
        E_ERROR("Invalid init value to create!");
        return true;
    }
    StaticSharedContext::instance().GetRegisterConnection().Send_CreateNewComponent(entity, componentValue);
    return false;
}


shared::ESharedError shared::CreateResource(EResourceData* data) 
{
    StaticSharedContext::instance().GetRegisterConnection().Send_AddResource(data);
    return false;
}

shared::ESharedError  shared::SetValue(EDataBase::Entity entity, const EString& valueIdent, const EString& valueString)
{
    StaticSharedContext::instance().GetRegisterConnection().Send_SetValue(entity, valueIdent, valueString);

    return false;
}

EVector<EDataBase::Entity> shared::GetAllEntites()
{
    return StaticSharedContext::instance().GetRegisterConnection().Send_GetAllEntites();
}


ERef<EProperty> shared::GetValueFromIdent(EDataBase::Entity entity, const EString& valueIdent) 
{
    ERef<EProperty> result = StaticSharedContext::instance().GetRegisterConnection().Send_GetValue(entity, valueIdent);
    if (!result)
    {
        E_WARN("Could not find value " + valueIdent);
    }
    
    return result;
}

shared::ESharedError shared::SetEnumValue(EDataBase::Entity entity, const EString& valueIdent, u32 value)
{
    EJson valueJson = EJson::object();
    valueJson["CurrentValue"] = value;

    StaticSharedContext::instance().GetRegisterConnection().Send_SetValue(entity, valueIdent, valueJson.dump());

    return false;
}


shared::ESharedError shared::AddArrayEntry(EDataBase::Entity entity, const EString& ident) 
{
    StaticSharedContext::instance().GetRegisterConnection().Send_AddArrayEntry(entity, ident);
    return false;
}

EVector<ERef<EProperty>> shared::GetAllComponents(EDataBase::Entity entity) 
{
    return StaticSharedContext::instance().GetRegisterConnection().Send_GetAllValues(entity);
}

ERef<EResourceData> shared::GetResource(EResourceData::t_ID id) 
{
    return StaticSharedContext::instance().GetRegisterConnection().Send_GetResourceData(id);
}

EVector<ERef<EResourceData>> shared::GetLoadedResource(const EString& resourceType) 
{
    return StaticSharedContext::instance().GetRegisterConnection().Send_GetAllResources(resourceType);
}

ESharedBuffer shared::GetRegisterAsBuffer()
{
    return StaticSharedContext::instance().GetRegisterConnection().Send_GetRegisterBuffer();
}


namespace Engine {
    
    namespace shared {

        StaticSharedContext::StaticSharedContext() 
            : fRegisterSocket(nullptr)
        {
            // Start socket connection. Windows needs permission. We start and end it all in this class
        #ifdef EWIN
            WSADATA wsaData;

            int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (res != 0)
            {
                E_ERROR("Could not start windows socket api!");
                E_ERROR("PROGRAMM NOT RUNNING CORRECTLY!");
                return;
            }
        #endif

            fRegisterConnection.GetEventDispatcher().ConnectAll([this](EStructProperty* property){
                fRegisterEventDispatcher.Post_P(property->GetDescription(), property);
            });
            fExtensionManager.GetEventDispatcher().ConnectAll([this](EStructProperty* property){
                fRegisterEventDispatcher.Post_P(property->GetDescription(), property);
            });

            fExtensionManager.GetTaskRegister().RegisterItem("Core", new ECFuncTask("CreateObject", [](){
                CreateEntity();
            }));

            // For now we create local socket
            fRegisterSocket = new ERegisterSocket(1420);
            fRegisterConnection.Init();
            fRegisterConnection.Connect("localhost", 1420);
        }

        StaticSharedContext::~StaticSharedContext() 
        {
            fRegisterConnection.CleanUp();

            if (fRegisterSocket) { delete fRegisterSocket; }
        #ifdef EWIN
            WSACleanup();
        #endif
        }

        EExtensionManager& StaticSharedContext::GetExtensionManager() 
        {
            return fExtensionManager;
        }
        
        ERegisterConnection& StaticSharedContext::GetRegisterConnection() 
        {
            return fRegisterConnection;
        }
        
        ERegisterEventDispatcher& StaticSharedContext::Events() 
        {
            return fRegisterEventDispatcher;
        }
        
        bool StaticSharedContext::IsLocaleServerRunning() const
        {
            return fRegisterSocket != nullptr && fRegisterSocket->IsRunning();
        }

        void StaticSharedContext::RestartLocaleServer(int port)
        {
            if (fRegisterSocket)
            {
                delete fRegisterSocket;
                fRegisterSocket = nullptr;
            }

            fRegisterSocket = new ERegisterSocket(port);
        }



        void StaticSharedContext::ConnectTo(const EString& address) 
        {
            // Restart the connection
            fRegisterConnection.CleanUp();

            if (address.length() == 0)
            {
                E_ERROR("No address to connect to!");
                if (fRegisterSocket)
                {
                    delete fRegisterSocket;
                    fRegisterSocket = nullptr;
                }
                fRegisterConnection.Init();
                return;
            }

            EVector<EString> addressStrings = EStringUtil::SplitString(address, ":");
            EString connectToAddress = addressStrings[0];
            int port = 1420;
            if (addressStrings.size() > 1)
            {
                EString portString = addressStrings[1];
                char* p;
                long newPort = strtol(portString.c_str(), &p, 10);
                if (p)
                {
                    port = (int) newPort;
                }
            }

            if (fRegisterSocket && connectToAddress != "localhost" && connectToAddress != "127.0.0.1")
            {
                delete fRegisterSocket;
                fRegisterSocket = nullptr;
            }
            fRegisterConnection.Init();
            
            E_INFO("Connecting to " + connectToAddress + ":" + std::to_string(port));
            fRegisterConnection.Connect(connectToAddress, port);
        }


        void StaticSharedContext::Start() 
        {
            fInstance = new StaticSharedContext();
        }

        void StaticSharedContext::Stop() 
        {
            delete fInstance;
            fInstance = nullptr;
        }

        StaticSharedContext& StaticSharedContext::instance() 
        {
            E_ASSERT_M(fInstance, "Shared Context was not created yet. You try and use the system before it was initilized. StaticSharedContext::Start() was not called!");
            return *fInstance;
        }
        
    }
}
