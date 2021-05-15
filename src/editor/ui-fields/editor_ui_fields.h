#pragma once

namespace Editor {

    class EExtensionView : public Engine::EUIField
    {
    private:
        Engine::EExtensionManager* fExtensionManager;
    public:
        EExtensionView(Engine::EExtensionManager* extenionManager);
        virtual ~EExtensionView();

        virtual bool OnRender() override;
    };

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
        void RenderStruct(Engine::EStructProperty* storage);
        void RenderPrimitive(Engine::EProperty* storage);

        void RenderBool(Engine::EValueProperty<bool>* storage);
        void RenderInteger(Engine::EValueProperty<i32>* storage);
        void RenderDouble(Engine::EValueProperty<double>* storage);
        void RenderString(Engine::EValueProperty<EString>* storage);
    };

}