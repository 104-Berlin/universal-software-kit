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
    private:
        EString fExtensionName;
        EString fFilePath;
#ifdef EWIN
        HINSTANCE fHandle;
#else
        void* fHandle;
#endif
    };

    E_STORAGE_STRUCT(EExtensionLoadedEvent,
        (EString, Extension)
    )

    E_STORAGE_STRUCT(EExtensionUnloadEvent,
        (EString, ExtensionName),
        (EString, PathToExtension)
    )

    class E_EXTAPI EExtensionManager
    {
    private:
        EUnorderedMap<EString, EExtension*> fLoadedExtensions;
        ERegister*                          fLoadedScene;
        ETypeRegister                       fTypeRegister;
        EResourceRegister                   fResourceRegister;
        EEventDispatcher                    fEventDispatcher;
        EChaiContext                        fChaiScriptContext;
    public:
        EExtensionManager();
        ~EExtensionManager();

        /**
         * 
         * Loads and register new extension
         * @param pathToExtension
         * @return Success or Fail
         */
        bool LoadExtension(const EString& pathToExtension);

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
         * @return The active scene
         */
        ERegister* GetActiveScene() const;

        /**
         * Gets a value description by name
         */
        EValueDescription GetValueDescriptionById(const EString& extensionName, const EString& typeId);


        ETypeRegister& GetTypeRegister();
        const ETypeRegister& GetTypeRegister() const;

        EResourceRegister& GetResourceRegister();
        const EResourceRegister& GetResourceRegister() const;


        EChaiContext& GetChaiContext();
        const EChaiContext& GetChaiContext() const;

        static EExtensionManager& instance();


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