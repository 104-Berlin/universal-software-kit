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

    private:
        EString fExtensionName;
#ifdef EWIN
        HINSTANCE fHandle;
#else
        void* fHandle;
#endif
    };

    E_STORAGE_STRUCT(EExtensionLoadedEvent,
        (EString, Extension)
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

        template <typename EventType, typename CB>
        void AddEventListener(CB&& callback)
        {
            fEventDispatcher.Connect<EventType>(callback);
        }
    };

}