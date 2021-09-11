#include "prefix_interface.h"

// Some helpers for quicker function calls
#define EXTENSION_MANAGER StaticSharedContext::instance().GetExtensionManager()
#define ACTIVE_SCENE StaticSharedContext::instance().GetExtensionManager().GetActiveScene()

using namespace Engine;

shared::StaticSharedContext* shared::StaticSharedContext::fInstance = nullptr;

shared::ESharedError shared::LoadExtension(const EString& pathToExtension) 
{
    EFile file(pathToExtension);
    if (!file.Exist())
    {
        return true; // ERROR
    }
    if (!EXTENSION_MANAGER.LoadExtension(pathToExtension))
    {
        return true; // ERROR
    }
    EExtension* extension = EXTENSION_MANAGER.GetExtension(file.GetFileName());
    E_ASSERT(extension);
    E_INFO("Loaded extension \"" + extension->GetName() + "\"");

    return false;
}

shared::ESharedError shared::CreateEntity() 
{
    ERegister::Entity ent = ACTIVE_SCENE->CreateEntity();
    E_INFO("Created entity " + std::to_string(ent));
    return false;
}

shared::ESharedError shared::CreateComponent(const EString& componentId, ERegister::Entity entity) 
{
    EValueDescription desc;
    if (!EXTENSION_MANAGER.GetTypeRegister().FindItem(EFindTypeDescByName(componentId), &desc))
    {
        E_ERROR("Could not find type");
        return true; // ERROR
    }
    EStructProperty* prop = ACTIVE_SCENE->AddComponent(entity, desc);
    if (!prop) 
    {
        return true; // ERROR
    }
    inter::PrintProperty(prop);

    return false;
}


namespace Engine {
    
    namespace shared {

        StaticSharedContext::StaticSharedContext() 
        {
            
        }

        StaticSharedContext::~StaticSharedContext() 
        {
            
        }

        EExtensionManager& StaticSharedContext::GetExtensionManager() 
        {
            return fExtensionManager;
        }


        void StaticSharedContext::Start() 
        {
            fInstance = new StaticSharedContext();
        }

        void StaticSharedContext::CleanUp() 
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