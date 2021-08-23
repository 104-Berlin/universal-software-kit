#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

EResourceManager::EResourceManager() 
{
    
}

EResourceManager::~EResourceManager() 
{
    
}

bool EResourceManager::HasResource(const EString& path) const
{
    return fLoadedResources.find(path) != fLoadedResources.end();
}

void EResourceManager::RegisterResource(const EString& type, const EString& path, byte* resourceData, size_t resourceDataSize) 
{
    E_INFO("New Resource: " + path);
    fLoadedResources.insert({path, EResourceData(type, path, resourceData, resourceDataSize)});
}

EResourceData EResourceManager::GetResource(const EString& path) const
{
    if (HasResource(path))
    {
        return fLoadedResources.at(path);
    }
    return EResourceData();
}

EVector<EResourceData> EResourceManager::GetAllResource() const
{
    EVector<EResourceData> result;
    for (auto& entry : fLoadedResources)
    {
        result.push_back(entry.second);
    }
    return result;
}

EVector<EResourceData> EResourceManager::GetAllResource(const EString& type) const
{
    EVector<EResourceData> result;

    for (auto& entry : fLoadedResources)
    {
        if (entry.second.Type == type)
        {
                
        }
    }

    return result;
}
