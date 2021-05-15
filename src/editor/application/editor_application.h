#pragma once

namespace Editor {

    class EApplication
    {
    private:
        Graphics::GContext* fGraphicsContext;

        EVector<ERef<Engine::EUIPanel>> fDefaultPanels;
        ERef<Engine::EUIMainMenuBar>            fMainMenu;

        Engine::EExtensionManager   fExtensionManager;
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