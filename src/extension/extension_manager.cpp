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
    fFilePath = pluginFile.GetFullPath();

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

const EString& EExtension::GetFilePath() const
{
    return fFilePath;
}


EExtensionManager::EExtensionManager()
{
    fLoadedScene = new ERegister("New Scene");
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

    fEventDispatcher.Post<EExtensionLoadedEvent>({newExtension->GetName()});
    return true;
}

EExtension* EExtensionManager::GetExtension(const EString& extensionName)
{
    if (!IsLoaded(extensionName)) { E_WARN("Could not find extension \"" + extensionName + "\""); return nullptr; }
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

bool EExtensionManager::IsLoaded(const EString& extensionName) 
{
    return fLoadedExtensions.find(extensionName) != fLoadedExtensions.end();
}

ERegister* EExtensionManager::GetActiveScene() const
{
    return fLoadedScene;
}

EValueDescription EExtensionManager::GetValueDescriptionById(const EString& extensionName, const EString& typeId) 
{
    const EVector<EValueDescription>& registeredTypes = fTypeRegister.GetItems(extensionName);
    for (EValueDescription dsc : registeredTypes)
    {
        if (dsc.GetId() == typeId)
        {
            return dsc;
        }
    }
    return EValueDescription();
}

ETypeRegister& EExtensionManager::GetTypeRegister() 
{
    return fTypeRegister;
}

const ETypeRegister& EExtensionManager::GetTypeRegister() const
{
    return fTypeRegister;
}

EResourceRegister& EExtensionManager::GetResourceRegister() 
{
    return fResourceRegister;
}

const EResourceRegister& EExtensionManager::GetResourceRegister() const
{
    return fResourceRegister;
}

EExtensionManager& EExtensionManager::instance() 
{
    static EExtensionManager theManager;
    return theManager;
}

void EExtensionManager::Reload() 
{
    // Cache pointers to extension, because map will change with reload
    fLoadedScene->DisconnectEvents();

    EVector<EExtension*> allExtensions = GetLoadedExtensions();
    EVector<EString> extensionToLoad;
    for (EExtension* ext : allExtensions)
    {
        extensionToLoad.push_back(ext->GetFilePath());
        UnloadExtension(ext);
    }

    delete fLoadedScene;
    fLoadedScene = new ERegister();

    for (const EString& extensionPath : extensionToLoad)
    {
        LoadExtension(extensionPath);
    }
}

void EExtensionManager::ReloadExtension(const EString& extensionName) 
{
    EExtension* toReload = GetExtension(extensionName);
    if (toReload)
    {
        ReloadExtension(toReload);
    }
    else
    {
        E_WARN("Could not reload extension " + extensionName + ". It was not found!");
    }
}

void EExtensionManager::ReloadExtension(EExtension* extension) 
{
    EString extensionPath = extension->GetFilePath();
    UnloadExtension(extension);
    // TODO: We have to find all events that got queued be the extension.
    // If we call these functions they will be deleted i guess
    LoadExtension(extensionPath);
}

void EExtensionManager::UnloadExtension(EExtension* extension) 
{
    if (!extension) { return; }
    EString extensionName = extension->GetName();
    EString extensionPath = extension->GetFilePath();

    fTypeRegister.ClearRegisteredItems(extensionName);
    fResourceRegister.ClearRegisteredItems(extensionName);
    fLoadedExtensions.erase(extensionName);
    fEventDispatcher.Post<EExtensionUnloadEvent>({extensionName, extensionPath});

    delete extension;
}
