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


shared::ESharedError shared::CreateComponent(const EString& componentId, ERegister::Entity entity) 
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

shared::ESharedError shared::CreateComponent(const EValueDescription& componentId, ERegister::Entity entity) 
{
    if (componentId.GetType() != EValueType::STRUCT)
    {
        E_ERROR("You have to add struct as component");
        return true;
    }
    StaticSharedContext::instance().GetRegisterConnection().Send_CreateNewComponent(entity, static_cast<EStructProperty*>(EProperty::CreateFromDescription(componentId.GetId(), componentId)));
    return false;
}

shared::ESharedError shared::CreateComponent(EStructProperty* componentValue, ERegister::Entity entity)
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

shared::ESharedError  shared::SetValue(ERegister::Entity entity, const EString& valueIdent, const EString& valueString)
{
    StaticSharedContext::instance().GetRegisterConnection().Send_SetValue(entity, valueIdent, valueString);

    return false;
}


ERef<EProperty> shared::GetValueFromIdent(ERegister::Entity entity, const EString& valueIdent) 
{
    ERef<EProperty> result = StaticSharedContext::instance().GetRegisterConnection().Send_GetValue(entity, valueIdent);
    if (!result)
    {
        E_WARN("Could not find value " + valueIdent);
    }
    
    return result;
}

shared::ESharedError shared::AddArrayEntry(ERegister::Entity entity, const EString& ident) 
{
    StaticSharedContext::instance().GetRegisterConnection().Send_AddArrayEntry(entity, ident);
    return false;
}

EVector<ERef<EProperty>> shared::GetAllComponents(ERegister::Entity entity) 
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


            // For now we create local socket
            fRegisterSocket = new ERegisterSocket(420);
            fRegisterConnection.Init();
            fRegisterConnection.Connect("localhost", 420);
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
        
        void StaticSharedContext::ConnectTo(const EString& address) 
        {
            // Restart the connection
            fRegisterConnection.CleanUp();
            if (fRegisterSocket)
            {
                delete fRegisterSocket;
                fRegisterSocket = nullptr;
            }
            fRegisterConnection.Init();
            
            E_INFO("Connecting to " + address);
            fRegisterConnection.Connect(address, 420);
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
