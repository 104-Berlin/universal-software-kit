#include "prefix_interface.h"

// Some helpers for quicker function calls
#define EXTENSION_MANAGER StaticSharedContext::instance().GetExtensionManager()
#define ACTIVE_SCENE StaticSharedContext::instance().GetExtensionManager().GetActiveScene()

using namespace Engine;

shared::StaticSharedContext* shared::StaticSharedContext::fInstance = nullptr;

shared::ESharedError shared::LoadExtension(const EString& pathToExtension) 
{
    StaticSharedContext::instance().RunInMainThread([pathToExtension](){
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
    });

    return false;
}

shared::ESharedError shared::CreateEntity() 
{
    StaticSharedContext::instance().RunInMainThread([](){
        ERegister::Entity ent = ACTIVE_SCENE->CreateEntity();
        E_INFO("Created entity " + std::to_string(ent));
    });
    return false;
}

shared::ESharedError shared::CreateComponent(const EString& componentId, ERegister::Entity entity) 
{
    StaticSharedContext::instance().RunInMainThread([componentId, entity](){
        EValueDescription desc;
        if (!EXTENSION_MANAGER.GetTypeRegister().FindItem(EFindTypeDescByName(componentId), &desc))
        {
            E_ERROR("Could not find type");
            return; // ERROR
        }
        EStructProperty* prop = ACTIVE_SCENE->AddComponent(entity, desc);
        if (!prop) 
        {
            return; // ERROR
        }
        inter::PrintProperty(prop);
    });
    return false;
}


namespace Engine {
    
    namespace shared {

        StaticSharedContext::StaticSharedContext() 
        {
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
            if (fRunningThread.joinable())
            {
                fRunningThread.join();
            }
        }

        EExtensionManager& StaticSharedContext::GetExtensionManager() 
        {
            return fExtensionManager;
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