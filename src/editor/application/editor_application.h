#pragma once

namespace Editor {

    class EApplication
    {
    private:
        Graphics::GContext* fGraphicsContext;

        ERef<Engine::EUIMainMenuBar>            fMainMenu;

        Engine::EUIRegister         fUIRegister;
        ECommandLine                fCommandLine;
        EString                     fLoadOnStartRegister;
    public:
        EApplication();

        void Start(const EString& defaultRegisterPath = "");
        void RegenerateMainMenuBar();

        static bool LoadRegisterFromFile(const EString& path);
        static bool SaveRegisterToFile(const EString& path);
    private:
        void Init(Graphics::GContext* context);
        void CleanUp();
        void Render();
        void RenderImGui();

        void RegisterDefaultPanels();
    };

}