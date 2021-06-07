#include "engine_extension.h"

using namespace Engine;

EExtension::EExtension(const EString& pathToPlugin) 
{
    EFile pluginFile(pathToPlugin);

    if (!pluginFile.Exist())
    {
        E_ERROR("Plugin file \"" + pluginFile.GetFileName() + "\" does not exist!");
        return;
    }

    fExtensionName = pluginFile.GetFileName();

    LoadPlugin(pluginFile.GetFullPath());
}

EExtension::~EExtension() 
{
    auto cleanFunction = (void(*)())GetFunction("cleanUp");
    if (cleanFunction)
    {
        cleanFunction();
    }
    #ifdef EWIN
    FreeLibrary(fHandle);
    #else
    dlclose(fHandle);
    #endif
}

void EExtension::LoadPlugin(const EString& fullPath) 
{   
#ifdef EWIN
    fHandle = LoadLibrary(TEXT(fullPath.c_str()));
#else
    fHandle = dlopen(fullPath.c_str(), RTLD_LAZY);
#endif
    if (!fHandle) 
    {
        std::cout << "Could not load plugin \"" << fullPath << "\"!" << std::endl;
    }
}

void* EExtension::GetFunction(const EString& functionName) 
{
#ifdef EWIN
    return GetProcAddress(fHandle, functionName.c_str());
#else
    void* result = dlsym(fHandle, functionName.c_str());
    return result;
#endif
}

void EExtension::InitImGui() 
{
    auto loadFunction = (void(*)())GetFunction("InitImGui");
    if (loadFunction)
    {
        loadFunction();
    }
}

const EString& EExtension::GetName() const
{
    return fExtensionName;
}


EExtensionManager::EExtensionManager()
{
    fLoadedScene = new EScene("New Scene");
}

EExtensionManager::~EExtensionManager() 
{
    delete fLoadedScene;
}

bool EExtensionManager::LoadExtension(const EString& pathToExtensio)
{
    EFile file(pathToExtensio);
    if (!file.Exist()) { E_ERROR("Could not find Plugin File \"" + file.GetFullPath() + "\""); return false; }

    if (fLoadedExtensions.find(file.GetFileName()) != fLoadedExtensions.end())
    {
        E_ERROR("Error loading extension \"" + file.GetFileName() + "\"! An extension with the same name is already loaded!");
        return false;
    }

    EExtension* newExtension = new EExtension(file.GetFullPath());
    newExtension->InitImGui();
    // Run load function
    
    auto loadFunction = (void(*)(const char*, EExtensionManager&))newExtension->GetFunction("entry");
    if (loadFunction)
    {
        E_INFO("Running load function for plugin \"" + newExtension->GetName() + "\"");
        loadFunction(newExtension->GetName().c_str(), *this);
    }
    fLoadedExtensions[newExtension->GetName()] = newExtension;
    EExtensionLoadedEvent evt;
    evt.Extension = newExtension;
    fEventDispatcher.Post<EExtensionLoadedEvent>(evt);
    return true;
}

EExtension* EExtensionManager::GetExtension(const EString& extensionName)
{
    return fLoadedExtensions[extensionName];
}

EVector<EExtension*> EExtensionManager::GetLoadedExtensions()
{
    EVector<EExtension*> result;
    for (auto& entry : fLoadedExtensions)
    {
        result.push_back(entry.second);
    }
    return result;
}

EScene* EExtensionManager::GetActiveScene() const
{
    return fLoadedScene;
}

ERef<EValueDescription> EExtensionManager::GetValueDescriptionById(const EString& extensionName, const EString& typeId) 
{
    const EVector<ERef<EValueDescription>>& registeredTypes = fTypeRegister.GetItems(extensionName);
    for (ERef<EValueDescription> dsc : registeredTypes)
    {
        if (dsc->GetId() == typeId)
        {
            return dsc;
        }
    }
    return nullptr;
}

ETypeRegister& EExtensionManager::GetTypeRegister() 
{
    return fTypeRegister;
}

const ETypeRegister& EExtensionManager::GetTypeRegister() const
{
    return fTypeRegister;
}