#pragma once

namespace Editor {

    class EObjectView : public Engine::EUIField
    {
    private:
        EVector<ERef<Engine::EProperty>>    fSelectedComponents;
        Engine::ERegister::Entity           fSelectedEntity;
        EVector<Engine::ERegister::Entity>  fEntities;
        EWeakRef<Engine::EUITable>          fEntitiesTable;
        EWeakRef<Engine::EUIContainer>      fComponentsView;
        EUnorderedMap<Engine::ERegister::Entity, EWeakRef<Engine::EUITableRow>> fEntityRows;

        std::mutex                          fChangeComponentsMtx;
    public:
        EObjectView();

        virtual bool OnRender() override;

        virtual void OnUpdateEventDispatcher() override;
    private:
        ERef<EUIField> RenderProperty(Engine::EProperty* storage, EString nameIdent);

        ERef<EUIField> RenderStruct(Engine::EStructProperty* storage, EString nameIdent);
        ERef<EUIField> RenderPrimitive(Engine::EProperty* storage, EString nameIdent);
        ERef<EUIField> RenderEnum(Engine::EEnumProperty* storage, EString nameIdent);
        ERef<EUIField> RenderArray(Engine::EArrayProperty* storage, EString nameIdent);

        ERef<EUIField> RenderBool(Engine::EValueProperty<bool>* storage, EString nameIdent);
        ERef<EUIField> RenderInteger(Engine::EValueProperty<i32>* storage, EString nameIdent);
        ERef<EUIField> RenderInteger(Engine::EValueProperty<u32>* storage, EString nameIdent);
        ERef<EUIField> RenderInteger(Engine::EValueProperty<u64>* storage, EString nameIdent);
        ERef<EUIField> RenderDouble(Engine::EValueProperty<double>* storage, EString nameIdent);
        ERef<EUIField> RenderString(Engine::EValueProperty<EString>* storage, EString nameIdent);

        bool HasSelectedComponent(Engine::EValueDescription dsc);

        void RegenComponentsView();
    };

}