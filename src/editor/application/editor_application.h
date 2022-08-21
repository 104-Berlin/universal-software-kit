#pragma once

namespace Editor {

    E_STORAGE_STRUCT(ApplicationState,
        (EVector<EString>, AutoLoadExtensions)
    )

    class EApplication
    {
    private:
        Graphics::GContext* fGraphicsContext;
        
        ERef<Engine::EUIMainMenuBar>            fMainMenu;

        Engine::EUIRegister         fUIRegister;
        Engine::EUIValueRegister    fUIValueRegister;
        Engine::EViewportRenderFunctionRegister fViewportRenderFunctionRegister;
        Engine::EUIViewportManager  fViewportManager;

        ECommandLine                fCommandLine;
        EString                     fLoadOnStartRegister;
        EUnorderedMap<Engine::ERegister::Entity, Renderer::RObject*> fEntityObjectMap;
    public:
        EApplication();
        ~EApplication();

        void Start(const EString& defaultRegisterPath = "");
        void RegenerateMainMenuBar();

        Engine::EUIValueRegister& GetUIValueRegister() { return fUIValueRegister; }

        bool ExtensionDefaultLoad() const;
    private:
        void Init(Graphics::GContext* context);
        void CleanUp();
        void Render();
        void RenderImGui();

        void RegisterDefaultPanels();
        void RegisterDefaultResources();
        void RegisterDefaultComponentRender();

        ApplicationState CreateApplicationState() const;

        void SaveApplicationState() const;
        void LoadApplicationState();
    public:
        static EApplication* gApp();
    };

}