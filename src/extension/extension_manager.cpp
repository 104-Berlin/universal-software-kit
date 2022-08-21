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

void EExtension::SetAutoLoad(bool autoLoad)
{
    fAutoLoad = autoLoad;
}

bool EExtension::GetAutoLoad() const
{
    return fAutoLoad;
}

EExtensionManager::EExtensionManager()
{
}

EExtensionManager::~EExtensionManager() 
{
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
    newExtension->SetAutoLoad(IsAutoLoad(newExtension->GetName()));
    newExtension->InitImGui();
    // Run load function
    
    auto loadFunction = (void(*)(const char*, EExtensionManager&))newExtension->GetFunction("entry");
    if (loadFunction)
    {
        E_INFO("Running load function for plugin \"" + newExtension->GetName() + "\"");
        loadFunction(newExtension->GetName().c_str(), *this);
    }
    fLoadedExtensions[newExtension->GetName()] = newExtension;

    fEventDispatcher.Post<events::EExtensionLoadedEvent>({newExtension->GetName()});
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

void EExtensionManager::SetExtensionAutoLoad(const EString& extensionName, bool autoLoad)
{
    if (autoLoad)
    {
        fAutoLoadExtensions.insert(extensionName);
    }
    else
    {
        fAutoLoadExtensions.erase(extensionName);
    }
    if (!IsLoaded(extensionName)) { E_WARN("Could not find extension \"" + extensionName + "\""); return; }

    fLoadedExtensions[extensionName]->SetAutoLoad(autoLoad);
}

bool EExtensionManager::IsAutoLoad(const EString& extensionName)
{
    if (fAutoLoadExtensions.find(extensionName) != fAutoLoadExtensions.end())
    {
        return true;
    }
    if (!IsLoaded(extensionName))
    { 
        return false; 
    }
    return fLoadedExtensions[extensionName]->GetAutoLoad();
}


EValueDescription EExtensionManager::GetValueDescriptionById(const EString& extensionName, const EString& typeId) 
{
    const EVector<EComponentRegisterEntry>& registeredTypes = fTypeRegister.GetItems(extensionName);
    for (EComponentRegisterEntry dsc : registeredTypes)
    {
        if (dsc.Description.GetId() == typeId)
        {
            return dsc.Description;
        }
    }
    return EValueDescription();
}

EComponentRegister& EExtensionManager::GetComponentRegister() 
{
    return fTypeRegister;
}

const EComponentRegister& EExtensionManager::GetComponentRegister() const
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

ETaskRegister& EExtensionManager::GetTaskRegister()
{
    return fTaskRegister;
}

const ETaskRegister& EExtensionManager::GetTaskRegister() const
{
    return fTaskRegister;
}

EEventDispatcher& EExtensionManager::GetEventDispatcher()
{
    return fEventDispatcher;
}

const EEventDispatcher& EExtensionManager::GetEventDispatcher() const
{
    return fEventDispatcher;
}

void EExtensionManager::Reload() 
{
    // Cache pointers to extension, because map will change with reload
    EVector<EExtension*> allExtensions = GetLoadedExtensions();
    EVector<EString> extensionToLoad;
    for (EExtension* ext : allExtensions)
    {
        extensionToLoad.push_back({ext->GetFilePath(), ext->GetAutoLoad()});
        UnloadExtension(ext);
    }

    for (const auto& exInfo : extensionToLoad)
    {
        LoadExtension(exInfo);
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
    fTaskRegister.ClearRegisteredItems(extensionName);
    
    fEventDispatcher.Post<events::EExtensionUnloadEvent>({extensionName, extensionPath});

    fLoadedExtensions.erase(extensionName);
    delete extension;
}
