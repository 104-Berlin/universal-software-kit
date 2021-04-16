#pragma once

namespace Engine {

    struct E_EXTAPI EExtInitInfo
    {
        EUIRegister*    UIRegister = nullptr;
    };

    class E_EXTAPI EExtension
    {
        friend class EExtensionManager;
    public:
        EExtension(const EString& pathToPlugin);
        ~EExtension();
    private:
        void    LoadPlugin(const EString& fullPath);
        void*   GetFunction(const EString& functionName);
    public:
        void InitImGui();

        const EString& GetName() const;
    private:
        EString fExtensionName;
#ifdef EWIN
        HINSTANCE fHandle;
#else
        void* fHandle;
#endif
    };

    class E_EXTAPI EExtensionManager
    {
    private:
        EUnorderedMap<EString, EExtension*> fLoadedExtensions;
        EExtInitInfo                        fExtensionRegisters;
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
    };

}