#pragma once

namespace Editor {

    class EObjectView : public Engine::EUIField
    {
    private:
        Engine::EScene::Entity fSelectedEntity;
        Engine::EScene* fScene;
        ERef<Engine::EUIButton> fAddObjectButton;
    public:
        EObjectView(Engine::EScene* scene);

        virtual bool OnRender() override;

        virtual void OnUpdateEventDispatcher() override;
    private:
        void RenderComponentStorage(Engine::EComponentStorage storage);

        void RenderBool(const EString& name, Engine::EComponentStorage storage);
        void RenderDouble(const EString& name, Engine::EComponentStorage storage);
        void RenderInteger(const EString& name, Engine::EComponentStorage storage);
        void RenderString(const EString& name, Engine::EComponentStorage storage);
    };

}