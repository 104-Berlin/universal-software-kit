#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

EResourceManager::EResourceManager() 
{
    
}

EResourceManager::~EResourceManager() 
{
    Clear();
}

bool EResourceManager::HasResource(const EResourceData::t_ID& id) const
{
    return fLoadedResources.find(id) != fLoadedResources.end();
}

bool EResourceManager::AddResource(EResourceData* data) 
{
    if (HasResource(data->ID))
    {
        return false;
    }
    fLoadedResources.insert({data->ID, data});
    return true;
}

bool EResourceManager::ImportResource(const EString& name, const EResourceDescription& description, u8* rawData, size_t data_size) 
{
    EResourceDescription::ResBuffer buffer;
    buffer.Data = rawData;
    buffer.Size = data_size;
    if (description.ImportFunction)
    {
        buffer = description.ImportFunction({rawData, data_size});
        if (!buffer.Data)
        {
            E_ERROR("Resource " + name + " could not be imported!");
            return false;
        }
    }
    EResourceData* newData = new EResourceData(CreateNewId(), description.ResourceName, name, buffer.Data, buffer.Size);
    if (buffer.UserData)
    {
        newData->SetUserData(buffer.UserData, buffer.UserDataSize);
    }
    fLoadedResources.insert({newData->ID, newData});
    return true;
}

bool EResourceManager::ImportResourceFromFile(const EString& filePath, const EResourceDescription& description) 
{
    EFile file(filePath);
    return ImportResourceFromFile(file, description);
}

bool EResourceManager::ImportResourceFromFile(EFile& file, const EResourceDescription& description) 
{
    E_ASSERT(file.Exist());
    file.LoadToMemory();
    size_t file_size = file.GetBuffer().GetSizeInByte();



    u8* dataPtr = nullptr;
    size_t dataSize = 0;
    if (description.ImportFunction)
    {
        dataPtr = file.GetBuffer().Data<u8>();
        dataSize = file_size;
    }
    else
    {
        dataPtr = new u8[file_size];
        memcpy(dataPtr, file.GetBuffer().Data(), file_size);
        dataSize = file_size;
    }
    return ImportResource(file.GetFileName(), description, dataPtr, dataSize);
}

void EResourceManager::Clear() 
{
    for (auto& entry : fLoadedResources)
    {
        delete entry.second;
    }
    fLoadedResources.clear();
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
