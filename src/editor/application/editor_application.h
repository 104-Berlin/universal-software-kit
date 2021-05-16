#pragma once

namespace Editor {

    class EApplication
    {
    private:
        Graphics::GContext* fGraphicsContext;

        ERef<Engine::EUIMainMenuBar>            fMainMenu;

        Engine::EExtensionManager   fExtensionManager;
        Engine::EUIRegister         fUIRegister;
    public:
        EApplication();

        void Start();
        void RegenerateMainMenuBar();
    private:
        void Init(Graphics::GContext* context);
        void CleanUp();
        void Render();
        void RenderImGui();

        void RegisterDefaultPanels();
    };

}