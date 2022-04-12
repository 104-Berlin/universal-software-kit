#pragma once

namespace Engine {

    class E_EXTAPI EExtension
    {
        friend class EExtensionManager;
    public:
        EExtension(const EString& pathToPlugin);
        ~EExtension();
    public:
        void    LoadPlugin(const EString& fullPath);
        void*   GetFunction(const EString& functionName);
        void    InitImGui();

        const EString& GetName() const;
        const EString& GetFilePath() const;

        void SetAutoLoad(bool autoLoad);
        bool GetAutoLoad() const;
    private:
        EString fExtensionName;
        EString fFilePath;
#ifdef EWIN
        HINSTANCE fHandle;
#else
        void* fHandle;
#endif
        bool fAutoLoad;
    };

namespace events {

    E_STORAGE_STRUCT(EExtensionLoadedEvent,
        (EString, Extension)
    )

    E_STORAGE_STRUCT(EExtensionUnloadEvent,
        (EString, ExtensionName),
        (EString, PathToExtension)
    )
}

    class E_EXTAPI EExtensionManager
    {
    private:
        EUnorderedMap<EString, EExtension*> fLoadedExtensions;
        EHashSet<EString>                   fAutoLoadExtensions;
        EComponentRegister                  fTypeRegister;
        EResourceRegister                   fResourceRegister;
        EEventDispatcher                    fEventDispatcher;
        ETaskRegister                       fTaskRegister;
    public:
        EExtensionManager();
        ~EExtensionManager();

        /**
         * 
         * Loads and register new extension
         * @param pathToExtension
         * @return Success or Fail
         */
        bool LoadExtension(const EString& pathToExtension, bool autoLoad);

        /**
         * Get an extension by name
         * @param extensionName Name of the extension
         * @return Pointer to the extension data
         */
        EExtension* GetExtension(const EString& extensionName);

        /**
         * Gets all currently loaded extensions
         * @return Vector of all extensions
         */
        EVector<EExtension*> GetLoadedExtensions();

        /**
         * Check if an Extension is Loaded
         * @return IsLoaded
         */
        bool IsLoaded(const EString& extensionName);


        /**
         * @brief Set the Extension Auto Load
         * 
         * @param extensionName The extension
         * @param autoLoad Wether the extension should be auto loaded
         */
        void SetExtensionAutoLoad(const EString& extensionName, bool autoLoad);

        /**
         * @brief Check if Extension should load on startup
         * 
         * @param extensionName 
         * @return IsAutoLoaded
         */
        bool IsAutoLoad(const EString& extensionName);

        /**
         * Gets a value description by name
         */
        EValueDescription GetValueDescriptionById(const EString& extensionName, const EString& typeId);


        EComponentRegister& GetComponentRegister();
        const EComponentRegister& GetComponentRegister() const;

        EResourceRegister& GetResourceRegister();
        const EResourceRegister& GetResourceRegister() const;

        ETaskRegister& GetTaskRegister();
        const ETaskRegister& GetTaskRegister() const;

        EEventDispatcher& GetEventDispatcher();
        const EEventDispatcher& GetEventDispatcher() const;


        void Reload();
        void ReloadExtension(const EString& extensionName);
        void ReloadExtension(EExtension* extension);

        void UnloadExtension(EExtension* extension);

        template <typename EventType, typename CB>
        void AddEventListener(CB&& callback)
        {
            fEventDispatcher.Connect<EventType>(callback);
        }
    };

}
