#include "engine.h"
#include "prefix_util.h"

using namespace Engine;

EFolder::EFolder(const EString& path, i32 folderOptions) 
    : fFolderOptions(folderOptions)
{
    fFullPath = path;
    if (fFolderOptions & EFolderOptions_CreateIfNotExist && !Exist())
    {
        Create();
    }
}

EFolder::EFolder(EBaseFolder f, const EString& path, i32 folderOptions) 
    : EFolder(GetBaseFolderPath(f) + path, folderOptions)
{    
}

const EString& EFolder::GetFullPath() const
{
    return fFullPath;
}

bool EFolder::Exist() const
{
    return std::filesystem::exists(fFullPath);
}

void EFolder::Create() 
{
    std::filesystem::create_directories(GetFullPath());
}

std::filesystem::directory_iterator EFolder::Iterator()  const
{
    return std::filesystem::directory_iterator(GetFullPath());    
}

EString EFolder::GetBaseFolderPath(EBaseFolder f) 
{
    EString result = "";
    switch(f)
    {
    case EBaseFolder::APPLICATION:  result = std::filesystem::absolute(std::filesystem::path("Engine/")).lexically_normal().string(); break;
    case EBaseFolder::PLUGIN:       result = std::filesystem::absolute(std::filesystem::path("Engine/plugins/")).lexically_normal().string(); break;
    case EBaseFolder::RES:          result = std::filesystem::absolute(std::filesystem::path("Engine/res/")).lexically_normal().string(); break;
    }
    return result;
}