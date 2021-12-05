#pragma once

namespace Engine {

    namespace events {
        E_STORAGE_STRUCT(ComponentEditChangeEvent, 
            (EString, NameIdentifier)
        )
    }

}

namespace Editor {

    class EObjectView : public Engine::EUIField
    {
    private:
        Engine::EDataBase::Entity           fSelectedEntity;
        EWeakRef<Engine::EUITable>          fEntitiesTable;
        EWeakRef<Engine::EUIContainer>      fComponentsView;
        EUnorderedMap<Engine::EDataBase::Entity, EWeakRef<Engine::EUITableRow>> fEntityRows;

        Engine::EUIValueRegister* fUIValueRegister;
    public:
        EObjectView(Engine::EUIValueRegister* valueFieldRegister = nullptr);
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
        ERef<EUIField> RenderDouble(Engine::EValueProperty<float>* storage, EString nameIdent);
        ERef<EUIField> RenderString(Engine::EValueProperty<EString>* storage, EString nameIdent);

        void RegenComponentsView();
        void RegenAddComponentMenu();
    };

    
    class EComponentEdit : public Engine::EUIField
    {
        ERef<Engine::EProperty> fEditedProperty;
    public:
        EComponentEdit(const ERef<Engine::EProperty>& property);
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
        ERef<EUIField> RenderDouble(Engine::EValueProperty<float>* storage, EString nameIdent);
        ERef<EUIField> RenderString(Engine::EValueProperty<EString>* storage, EString nameIdent);

    };

}