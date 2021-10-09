#include "prefix_interface.h"

// Some helpers for quicker function calls
#define EXTENSION_MANAGER StaticSharedContext::instance().GetExtensionManager()
#define ACTIVE_SCENE StaticSharedContext::instance().GetExtensionManager().GetActiveScene()

using namespace Engine;

shared::StaticSharedContext* shared::StaticSharedContext::fInstance = nullptr;

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
    EValueDescription desc;
    if (!EXTENSION_MANAGER.GetTypeRegister().FindItem(EFindTypeDescByName(componentId), &desc))
    {
        E_ERROR("Could not find type" + componentId);
        return true; // ERROR
    }
    CreateComponent(desc, entity);
    return false;
}

shared::ESharedError shared::CreateComponent(const EValueDescription& componentId, ERegister::Entity entity) 
{
    StaticSharedContext::instance().GetRegisterConnection().Send_CreateNewComponent(entity, componentId);
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


ERef<EProperty> shared::GetValue(ERegister::Entity entity, const EString& valueIdent) 
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

EVector<ERef<EResourceData>> shared::GetLoadedResource() 
{
    return StaticSharedContext::instance().GetRegisterConnection().Send_GetAllResources();
}

ESharedBuffer shared::GetRegisterAsBuffer()
{
    return StaticSharedContext::instance().GetRegisterConnection().Send_GetRegisterBuffer();
}


namespace Engine {
    
    namespace shared {
        EEventDispatcher& ERegisterEventDispatcher::GetEventDispatcher() 
        {
            return fEventDispatcher;
        }
        
        const EEventDispatcher& ERegisterEventDispatcher::GetEventDispatcher() const
        {
            return fEventDispatcher;
        }
    

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
                fRegisterEventDispatcher.GetEventDispatcher().Post_P(property->GetDescription(), property);
            });
            fExtensionManager.GetEventDispatcher().ConnectAll([this](EStructProperty* property){
                fRegisterEventDispatcher.GetEventDispatcher().Post_P(property->GetDescription(), property);
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
