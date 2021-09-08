#pragma once

namespace Editor {

    class EExtensionView : public Engine::EUIField
    {
    public:
        EExtensionView();
        virtual ~EExtensionView();

        virtual bool OnRender() override;
    };

    class EResourceView : public Engine::EUIField
    {
    private:
        Engine::EResourceData::t_ID selectedResource;
    public:
        EResourceView();

        virtual bool OnRender() override;
    };

    class EObjectView : public Engine::EUIField
    {
    private:
        Engine::ERegister::Entity fSelectedEntity;
        ERef<Engine::EUIButton> fAddObjectButton;
    public:
        EObjectView();

        virtual bool OnRender() override;

        virtual void OnUpdateEventDispatcher() override;
    private:
        void RenderProperty(Engine::EProperty* storage);

        void RenderStruct(Engine::EStructProperty* storage);
        void RenderPrimitive(Engine::EProperty* storage);
        void RenderEnum(Engine::EEnumProperty* storage);
        void RenderArray(Engine::EArrayProperty* storage);

        void RenderBool(Engine::EValueProperty<bool>* storage);
        void RenderInteger(Engine::EValueProperty<i32>* storage);
        void RenderInteger(Engine::EValueProperty<u32>* storage);
        void RenderInteger(Engine::EValueProperty<u64>* storage);
        void RenderDouble(Engine::EValueProperty<double>* storage);
        void RenderString(Engine::EValueProperty<EString>* storage);
    };

    class ECommandLine : public Engine::EUIField
    {
    private:
        Engine::EChaiContext* fChaiContext;
    public:
        ECommandLine(Engine::EChaiContext* context);

        virtual bool OnRender() override;
    };

}