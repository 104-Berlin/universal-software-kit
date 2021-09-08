#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

EResourceManager::EResourceManager() 
{
    
}

EResourceManager::~EResourceManager() 
{
    
}

bool EResourceManager::HasResource(const EResourceData::t_ID& id) const
{
    return fLoadedResources.find(id) != fLoadedResources.end();
}

void EResourceManager::RegisterResource(const EString& type, const EString& path, byte* resourceData, size_t resourceDataSize) 
{
    E_INFO("New Resource: " + path);
    EFile f(path);
    
    fLoadedResources.insert({CreateNewId(), new EResourceData(type, f.GetFileName(), resourceData, resourceDataSize)});
}

EResourceData* EResourceManager::GetResource(const EResourceData::t_ID& path) const
{
    if (HasResource(path))
    {
        return fLoadedResources.at(path);
    }
    return nullptr;
}

EVector<EResourceData*> EResourceManager::GetAllResource() const
{
    EVector<EResourceData*> result;
    for (auto& entry : fLoadedResources)
    {
        result.push_back(entry.second);
    }
    return result;
}

EVector<EResourceData*> EResourceManager::GetAllResource(const EString& type) const
{
    EVector<EResourceData*> result;

    for (auto& entry : fLoadedResources)
    {
        if (entry.second->Type == type)
        {
            result.push_back(entry.second);
        }
    }

    return result;
}

EResourceData::t_ID EResourceManager::CreateNewId() 
{
    EResourceData::t_ID result = 1;
    while (HasResource(result) && result != 0)
    {
        result *= 3;
        result += 1;
        result = result << 2; // Just something kinda random. Maybe we find something better if needed
    }
    return result;
}
