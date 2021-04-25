#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

EScene::EScene(const EString& name)
    : fName(name)
{

}

EScene::~EScene()
{

}

EResourceManager& EScene::GetResourceManager() 
{
    return fResourceManager;
}

EObject::EObject(EScene* scene, entt::entity handle)
    : fScene(scene), fHandle(handle)
{

}

EObject EObject::Create(EScene* scene)
{
    E_ASSERT(scene, "Need Valid scene to create object!");

    entt::entity handle = scene->fRegistry.create();
    return EObject(scene, handle);
}

void EObject::Delete(EObject object) 
{
    E_ASSERT(object.fScene, "No scene to delete object from!");
    E_ASSERT(object.fHandle != entt::null, "Invalid object to Delete!");

    object.fScene->fRegistry.destroy(object.fHandle);
}