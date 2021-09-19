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
    /*StaticSharedContext::instance().RunInMainThread([](){
        ERegister::Entity ent = ACTIVE_SCENE->CreateEntity();
        E_INFO("Created entity " + std::to_string(ent));
    });*/
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
    StaticSharedContext::instance().GetRegisterConnection().Send_CreateNewComponent(entity, desc);

    /*StaticSharedContext::instance().RunInMainThread([componentId, entity](){
        EValueDescription desc;
        if (!EXTENSION_MANAGER.GetTypeRegister().FindItem(EFindTypeDescByName(componentId), &desc))
        {
            E_ERROR("Could not find type" + componentId);
            return; // ERROR
        }
        EStructProperty* prop = ACTIVE_SCENE->AddComponent(entity, desc);
        if (!prop) 
        {
            return; // ERROR
        }
        inter::PrintProperty(prop);
    });*/
    return false;
}

shared::ESharedError  shared::SetValue(ERegister::Entity entity, const EString& valueIdent, const EString& valueString)
{
    StaticSharedContext::instance().GetRegisterConnection().Send_SetValue(entity, valueIdent, valueString);
    /*StaticSharedContext::instance().RunInMainThread([valueIdent, entity, valueString](){
        EProperty* prop = ACTIVE_SCENE->GetValueByIdentifier(entity, valueIdent);
        if (!prop)
        {
            return;
        }

        EValueDescription desc = prop->GetDescription();
        

        EDeserializer::ReadPropertyFromJson(EJson::parse(valueString, [](int depth, EJson::parse_event_t event, EJson& parsed) -> bool { return true; }, false), prop);
    });*/
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

            // For now we create local socket
            fRegisterSocket = new ERegisterSocket(420);
            fRegisterConnection.Init();
            fRegisterConnection.Connect("localhost", 420);
            

            fRunningThread = std::thread([this](){
                fIsRunning = true;
                while (fIsRunning)
                {
                    if (fMainThreadQueue.size() > 0)
                    {
                        std::lock_guard<std::mutex> guard(fQueueMutex);
                        fMainThreadQueue.front()();
                        fMainThreadQueue.pop();
                    }
                    this->fExtensionManager.GetActiveScene()->UpdateEvents();
                }
            });
        }

        StaticSharedContext::~StaticSharedContext() 
        {
            fRegisterConnection.CleanUp();

            if (fRegisterSocket) { delete fRegisterSocket; }
        #ifdef EWIN
            WSACleanup();
        #endif
            if (fRunningThread.joinable())
            {
                fRunningThread.join();
            }
        }

        EExtensionManager& StaticSharedContext::GetExtensionManager() 
        {
            return fExtensionManager;
        }
        
        ERegisterConnection& StaticSharedContext::GetRegisterConnection() 
        {
            return fRegisterConnection;
        }
        
        void StaticSharedContext::ConnectTo(const EString& address) 
        {
            if (fRegisterSocket)
            {
                delete fRegisterSocket;
                fRegisterSocket = nullptr;
            }
            // Restart the connection
            fRegisterConnection.CleanUp();
            fRegisterConnection.Init();

            fRegisterConnection.Connect(address, 420);
        }
        
        void StaticSharedContext::RunInMainThread(std::function<void()> function) 
        {
            std::lock_guard<std::mutex> lock(fInstance->fQueueMutex);
            fInstance->fMainThreadQueue.push(function);
        }


        void StaticSharedContext::Start() 
        {
            fInstance = new StaticSharedContext();
        }

        void StaticSharedContext::Stop() 
        {
            fInstance->fIsRunning = false;
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