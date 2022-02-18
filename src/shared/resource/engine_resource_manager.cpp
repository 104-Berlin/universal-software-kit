#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;


EResourceBase::EResourceBase(const EString& resourceType)
    : ResourceType(resourceType)
{
    fName = "Unknown";
}

void EResourceBase::SaveToTempFile(const u8* data, const u32& data_size)
{
    ESharedBuffer buffer;
    buffer.InitWith<u8>(data, data_size);
}

void EResourceBase::SaveToTempFile(ESharedBuffer buffer)
{
    EFile file(EFile::GetTempPath());
    file.SetFileBuffer(buffer);
    file.SaveBufferToDisk();
}

const EString& EResourceBase::GetTempFilePath() const
{
    if (fTempFilePath.empty())
    {
        EFile file(Path::Join(EFile::GetTempPath(), fName + ".temp"));
        //fTempFilePath = file.GetFullPath();
        EResourceBase* self = (EResourceBase*)this;
        self->fTempFilePath = file.GetFullPath();
    }
    return fTempFilePath;
}

const EString& EResourceBase::GetName() const
{
    return fName;
}

void EResourceBase::SetName(const EString& name)
{
    fName = name;
}


EResource::EResource(const EString& type)
    : fID(0), fFileBuffer(), fType(type), fCPtr(nullptr), fPtrTypeHash(0)
{

}

void EResource::SetID(const EResourceBase::t_ID& id)
{
    fID = id;
}

const EResourceBase::t_ID& EResource::GetID() const
{
    return fID;
}

void EResource::SetBuffer(ESharedBuffer buffer)
{
    fFileBuffer = buffer;
}

const ESharedBuffer& EResource::GetBuffer() const
{
    return fFileBuffer;
}

void EResource::SetName(const EString& name)
{
    fName = name;
}

const EString& EResource::GetName() const
{
    return fName;
}


const EString& EResource::GetResourceType() const
{
    return fType;
}


EString EResource::GetTempFilePath() const
{
    return Path::Join(EFile::GetTempPath(), fName + ".temp");
}




EResourceManager::EResourceManager() 
{
    
}

EResourceManager::~EResourceManager() 
{
    Clear();
}

bool EResourceManager::HasResource(const EResourceBase::t_ID& id) const
{
    return fLoadedResources.find(id) != fLoadedResources.end();
}

bool EResourceManager::AddResource(EResource* data) 
{
    if (data->GetID() == 0)
    {
        data->SetID(CreateNewId());
    }
    if (HasResource(data->GetID()))
    {
        return false;
    }
    fLoadedResources.insert({data->GetID(), data});
    fEventDispacher.Post<events::EResourceAddedEvent>({data->GetID(), data->GetName(), data->GetTempFilePath(), data->GetResourceType()});
    return true;
}

bool EResourceManager::ImportResource(const EString& name, const EResourceDescription& description, u8* rawData, size_t data_size) 
{
    E_ASSERT_M(description.AcceptedFileEndings.size() > 0, "No accepted file endings specified!");
    ESharedBuffer buffer;
    buffer.InitWith<u8>(rawData, data_size);
    EFile file(Path::Join(EFile::GetTempPath(), name + description.AcceptedFileEndings[0]));
    file.SetFileBuffer(buffer);
    

    if (description.ImportFunction)
    {
        EResource* newResourceBase = description.ImportFunction(buffer);
        if (!newResourceBase)
        {
            E_ERROR("Resource " + name + " could not be imported!");
            return false;
        }
        newResourceBase->SetName(name);
        return AddResource(newResourceBase);
    }
    return false;
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

EResource* EResourceManager::GetResource(const EResourceBase::t_ID& path) const
{
    if (HasResource(path))
    {
        return fLoadedResources.at(path);
    }
    return nullptr;
}

EVector<EResource*> EResourceManager::GetAllResource() const
{
    EVector<EResource*> result;
    for (auto& entry : fLoadedResources)
    {
        result.push_back(entry.second);
    }
    return result;
}

EVector<EResource*> EResourceManager::GetAllResource(const EString& type) const
{
    EVector<EResource*> result;

    for (auto& entry : fLoadedResources)
    {
        if (entry.second->GetResourceType() == type)
        {
            result.push_back(entry.second);
        }
    }

    return result;
}

EResourceBase::t_ID EResourceManager::CreateNewId() 
{
    EResourceBase::t_ID result = 1;
    while (HasResource(result) && result != 0)
    {
        result *= 3;
        result += 1;
        result = result << 2; // Just something kinda random. Maybe we find something better if needed
    }
    return result;
}

EEventDispatcher& EResourceManager::GetEventDispatcher() 
{
    return fEventDispacher;
}

const EEventDispatcher& EResourceManager::GetEventDispatcher() const
{
    return fEventDispacher;
}

EResource* EResourceManager::CreateResourceFromFile(EFile& file, const EResourceDescription& description) 
{
    if (!file.Exist()) { return nullptr; }
    file.LoadToMemory();

    EResource* result = nullptr;

    if (description.ImportFunction)
    {
        result = description.ImportFunction(file.GetBuffer());
    }
    else
    {
        size_t bufferLen = file.GetBuffer().GetSizeInByte();
        u8* copiedFileBuffer = new u8[bufferLen];
        memcpy(copiedFileBuffer, file.GetBuffer().Data(), bufferLen);
        //result = new EResourceBase(0, description.ResourceName, file.GetFileName(), copiedFileBuffer, bufferLen);
    }
    if (result)
    {
        result->SetName(file.GetFileName());
    }
    return result;
}

EResource* EResourceManager::CreateResourceFromFile(const EString& filePath, const EResourceDescription& description) 
{
    EFile file(filePath);
    return CreateResourceFromFile(file, description);
}
