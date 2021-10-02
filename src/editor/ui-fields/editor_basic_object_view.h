#pragma once

namespace Editor {

    class EObjectView : public Engine::EUIField
    {
    private:
        EVector<ERef<Engine::EProperty>>    fSelectedComponents;
        Engine::ERegister::Entity           fSelectedEntity;
        EVector<Engine::ERegister::Entity>  fEntities;
        EWeakRef<Engine::EUITable>          fEntitiesTable;
        EUnorderedMap<Engine::ERegister::Entity, EWeakRef<Engine::EUITableRow>> fEntityRows;

        std::mutex                          fChangeComponentsMtx;
    public:
        EObjectView();

        virtual bool OnRender() override;

        virtual void OnUpdateEventDispatcher() override;
    private:
        void RenderProperty(Engine::EProperty* storage, EString nameIdent);

        void RenderStruct(Engine::EStructProperty* storage, EString nameIdent);
        void RenderPrimitive(Engine::EProperty* storage, EString nameIdent);
        void RenderEnum(Engine::EEnumProperty* storage, EString nameIdent);
        void RenderArray(Engine::EArrayProperty* storage, EString nameIdent);

        void RenderBool(Engine::EValueProperty<bool>* storage, EString nameIdent);
        void RenderInteger(Engine::EValueProperty<i32>* storage, EString nameIdent);
        void RenderInteger(Engine::EValueProperty<u32>* storage, EString nameIdent);
        void RenderInteger(Engine::EValueProperty<u64>* storage, EString nameIdent);
        void RenderDouble(Engine::EValueProperty<double>* storage, EString nameIdent);
        void RenderString(Engine::EValueProperty<EString>* storage, EString nameIdent);

        bool HasSelectedComponent(Engine::EValueDescription dsc);
    };

}