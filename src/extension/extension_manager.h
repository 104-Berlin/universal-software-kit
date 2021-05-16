#pragma once

namespace Engine {

    struct E_EXTAPI EExtInitInfo
    {
        
    };

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

    struct EExtensionLoadedEvent
    {
        EExtension* Extension;
    };

    class E_EXTAPI EExtensionManager
    {
    private:
        EUnorderedMap<EString, EExtension*> fLoadedExtensions;
        EExtInitInfo                        fExtensionRegisters;
        EScene*                             fLoadedScene;
        EEventDispatcher                    fEventDispatcher;
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

        const EExtInitInfo& GetRegisteres() const;

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


        EScene* GetActiveScene() const;


        template <typename EventType, typename CB>
        void AddEventListener(CB&& callback)
        {
            fEventDispatcher.Connect<EventType>(callback);
        }
    };

}