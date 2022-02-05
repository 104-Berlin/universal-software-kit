#pragma once

namespace Editor {

    class EApplication
    {
    private:
        Graphics::GContext* fGraphicsContext;

        ERef<Engine::EUIMainMenuBar>            fMainMenu;

        Engine::EUIRegister         fUIRegister;
        Engine::EUIValueRegister    fUIValueRegister;

        ECommandLine                fCommandLine;
        EString                     fLoadOnStartRegister;
    public:
        EApplication();
        ~EApplication();

        void Start(const EString& defaultRegisterPath = "");
        void RegenerateMainMenuBar();

        Engine::EUIValueRegister& GetUIValueRegister() { return fUIValueRegister; }
    private:
        void Init(Graphics::GContext* context);
        void CleanUp();
        void Render();
        void RenderImGui();

        void RegisterDefaultPanels();
        void RegisterDefaultResources();
        void RegisterDefaultComponentRender();

    public:
        static EApplication* gApp();
    };

}