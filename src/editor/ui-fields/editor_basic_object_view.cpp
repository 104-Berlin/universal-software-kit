#include "editor.h"

using namespace Editor;
using namespace Engine;

EObjectView::EObjectView(Engine::EUIValueRegister* valueFieldRegister)
    : EUIField("OBJECTVIEW"), fSelectedEntity(0), fUIValueRegister(valueFieldRegister)
{
    
    shared::Events().Connect<EntityCreateEvent>([this](EntityCreateEvent event){
        if (fEntitiesTable.expired()) { return; }
    
        ERef<EUITableRow> newRow = EMakeRef<EUITableRow>();
        fEntityRows.insert({event.Handle, newRow});
        fEntitiesTable.lock()->AddChild(newRow);

        EWeakRef<EUISelectable> selectabe = std::dynamic_pointer_cast<EUISelectable>(newRow->AddChild(EMakeRef<EUISelectable>(std::to_string(event.Handle))).lock());
        selectabe.lock()->SetStretchToAllColumns(true);

        selectabe.lock()->AddEventListener<events::ESelectionChangeEvent>([this, event](events::ESelectionChangeEvent se){
            if (se.Selected)
            {
                if (fSelectedEntity)
                {
                    std::dynamic_pointer_cast<EUISelectable>(fEntityRows[fSelectedEntity].lock()->GetChildAt(0).lock())->SetSelected(false);
                }

                fSelectedEntity = event.Handle;
            }
            else
            {
                fSelectedEntity = 0;
            }
            fComponentsView.lock()->SetDirty();
        });

        //newRow->AddChild(EMakeRef<EUILabel>("Some Text"));
    }, this);
    shared::Events().Connect<ComponentCreateEvent>([this](ComponentCreateEvent event){
        if (event.Handle == fSelectedEntity)
        {
            fComponentsView.lock()->SetDirty();
        }
    }, this);
    shared::Events().Connect<events::EExtensionLoadedEvent>([this](events::EExtensionLoadedEvent e){
        RegenAddComponentMenu();
    }, this);

    ERef<EUIContainer> entitiesList = EMakeRef<EUIContainer>("Entities");
    entitiesList->SetWidth(200);
    EWeakRef<EUIField> addObjectButton = entitiesList->AddChild(EMakeRef<EUIButton>("Add Object"));
    addObjectButton.lock()->AddEventListener<events::EButtonEvent>(&shared::CreateEntity);

    ERef<EUITable> table = EMakeRef<EUITable>();
    fEntitiesTable = table;
    table->SetHeaderCells({
        "ID",
        "More Info"
    });
    
    entitiesList->AddChild(table);



    ERef<EUIContainer> componentsList = EMakeRef<EUIContainer>("Components");
    fComponentsView = componentsList;

    fComponentsView.lock()->SetCustomUpdateFunction([this](){RegenComponentsView();});

    AddChild(entitiesList);
    AddChild(EMakeRef<EUISameLine>());
    AddChild(componentsList);


    RegenAddComponentMenu();
}

ERef<EUIField> EObjectView::RenderProperty(Engine::EProperty* storage, EString nameIdent) 
{
    EValueDescription propertyDsc = storage->GetDescription();
    EValueType type = propertyDsc.GetType();

    EUIValueRegisterEntry valueRender;
    if (fUIValueRegister && fUIValueRegister->FindItem(EFindValueFieldByType(propertyDsc.GetId()), &valueRender))
    {
        return valueRender.second(storage, fSelectedEntity, nameIdent);
    }

    switch (type)
    {
    case EValueType::STRUCT: return RenderStruct(static_cast<EStructProperty*>(storage), nameIdent); break;
    case EValueType::ARRAY: return RenderArray(static_cast<EArrayProperty*>(storage), nameIdent); break;
    case EValueType::PRIMITIVE: return RenderPrimitive(storage, nameIdent); break;
    case EValueType::ENUM: return RenderEnum(static_cast<EEnumProperty*>(storage), nameIdent); break;
    case EValueType::UNKNOWN: break;
    }
    return nullptr;
}

ERef<EUIField> EObjectView::RenderStruct(EStructProperty* storage, EString nameIdent) 
{
    ERef<EUIField> result = EMakeRef<EUIField>("STRUCT");
    EValueDescription description = storage->GetDescription();
    for (auto& entry : description.GetStructFields())
    {
        const EString& propertyName = entry.first;
        result->AddChild(RenderProperty(storage->GetProperty(propertyName), nameIdent + "." + propertyName));
    }
    return result;
}

ERef<EUIField> EObjectView::RenderPrimitive(Engine::EProperty* storage, EString nameIdent) 
{
    EValueDescription description = storage->GetDescription();
    const EString& primitiveId = description.GetId();
    if (primitiveId == E_TYPEID_STRING) {return  RenderString(static_cast<EValueProperty<EString>*>(storage), nameIdent); } 
    else if (primitiveId == E_TYPEID_INTEGER) { return RenderInteger(static_cast<EValueProperty<i32>*>(storage), nameIdent); }
    else if (primitiveId == E_TYPEID_UNSIGNED_INTEGER) { return RenderInteger(static_cast<EValueProperty<u32>*>(storage), nameIdent); }
    else if (primitiveId == E_TYPEID_UNSIGNED_BIG_INTEGER) { return RenderInteger(static_cast<EValueProperty<u64>*>(storage), nameIdent); }
    else if (primitiveId == E_TYPEID_DOUBLE) { return RenderDouble(static_cast<EValueProperty<double>*>(storage), nameIdent); }
    else if (primitiveId == E_TYPEID_FLOAT) { return RenderDouble(static_cast<EValueProperty<float>*>(storage), nameIdent); }
    else if (primitiveId == E_TYPEID_BOOL) { return RenderBool(static_cast<EValueProperty<bool>*>(storage), nameIdent); }
    return nullptr;
}

char* convert_str_to_chr(const std::string & s)
{
   char *pc = new char[s.size()+1];
   std::strcpy(pc, s.c_str());
   return pc; 
}

ERef<EUIField> EObjectView::RenderEnum(Engine::EEnumProperty* storage, EString nameIdent) 
{
    /*EValueDescription description = storage->GetDescription();
    int currentItem = -1;
    for (EString option : description.GetEnumOptions())
    {
        currentItem++;
        if (option == storage->GetCurrentValue())
        {
            break;
        }
    }
    std::vector<char*> opt;
    std::transform(description.GetEnumOptions().begin(), description.GetEnumOptions().end(), std::back_inserter(opt), convert_str_to_chr);

    ImGui::Combo(storage->GetPropertyName().c_str(), &currentItem, opt.data(), description.GetEnumOptions().size());

    for ( size_t i = 0 ; i < opt.size() ; i++ )
            delete [] opt[i];

    storage->SetCurrentValue(description.GetEnumOptions()[currentItem]);*/
    return nullptr;
}

ERef<EUIField> EObjectView::RenderArray(Engine::EArrayProperty* storage, EString nameIdent) 
{
    ERef<EUIField> field = EMakeRef<EUIField>("Some Label");
    EWeakRef<EUIField> weakRef = field;

    field->SetDirty();
    field->SetCustomUpdateFunction([this, weakRef, nameIdent](){
        ERef<EProperty> foundProp = shared::GetValue(fSelectedEntity, nameIdent);
        if (foundProp->GetDescription().GetType() != EValueType::ARRAY) { return; }
        weakRef.lock()->Clear();

        
        Engine::EArrayProperty* storage = (EArrayProperty*)foundProp.get();
        EVector<EProperty*> elements = storage->GetElements();

        for (size_t i = 0; i < elements.size(); i++)
        {
            EProperty* element = elements[i];
            weakRef.lock()->AddChild(RenderProperty(element, nameIdent + "." + std::to_string(i)));
        }

        EWeakRef<EUIField> addElemButton = weakRef.lock()->AddChild(EMakeRef<EUIButton>("Add Element"));
        addElemButton.lock()->AddEventListener<events::EButtonEvent>([this, nameIdent](){
            shared::AddArrayEntry(fSelectedEntity, nameIdent);
        });
    });



    shared::Events().AddEntityChangeEventListener(nameIdent, [weakRef](ERegister::Entity entity, const EString& valueIdent){
        if (weakRef.expired()) { return;}
        weakRef.lock()->SetDirty();
    }, field.get());
    return field;
}

ERef<EUIField> EObjectView::RenderBool(Engine::EValueProperty<bool>* storage, EString nameIdent) 
{
    ERef<EUICheckbox> result = EMakeRef<EUICheckbox>(storage->GetPropertyName().c_str());
    EWeakRef<EUICheckbox> weakResult = result;

    result->SetValue(storage->GetValue());

    result->AddEventListener<events::ECheckboxEvent>([this, nameIdent](events::ECheckboxEvent event){
        shared::SetValue<bool>(fSelectedEntity, nameIdent, event.Checked);
    });
    shared::Events().AddEntityChangeEventListener(nameIdent, [this, weakResult](ERegister::Entity entity, const EString& valueIdent){
        if (entity != fSelectedEntity) { return; }
        if (weakResult.expired()) { return; }
        ERef<EProperty> prop = shared::GetValue(entity, valueIdent);
        weakResult.lock()->SetValue(std::static_pointer_cast<EValueProperty<bool>>(prop)->GetValue());
    }, result.get());
    return result;
}

ERef<EUIField> EObjectView::RenderInteger(Engine::EValueProperty<i32>* storage, EString nameIdent) 
{
    ERef<EUIIntegerEdit> result = EMakeRef<EUIIntegerEdit>(storage->GetPropertyName().c_str());
    EWeakRef<EUIIntegerEdit> weakResult = result;

    result->SetValue(storage->GetValue());

    result->AddEventListener<events::EIntegerCompleteEvent>([this, nameIdent](events::EIntegerCompleteEvent event){
        shared::SetValue<i32>(fSelectedEntity, nameIdent, event.Value);
    });

    shared::Events().AddEntityChangeEventListener(nameIdent, [this, weakResult](ERegister::Entity entity, const EString& valueIdent){
        if (entity != fSelectedEntity) { return; }
        if (weakResult.expired()) { return; }
        ERef<EProperty> prop = shared::GetValue(entity, valueIdent);
        weakResult.lock()->SetValue(std::static_pointer_cast<EValueProperty<i32>>(prop)->GetValue());
    }, result.get());
    return result;
}

ERef<EUIField> EObjectView::RenderInteger(Engine::EValueProperty<u32>* storage, EString nameIdent) 
{
    ERef<EUIIntegerEdit> result = EMakeRef<EUIIntegerEdit>(storage->GetPropertyName().c_str());
    EWeakRef<EUIIntegerEdit> weakResult = result;

    result->SetValue(storage->GetValue());

    result->AddEventListener<events::EIntegerCompleteEvent>([this, nameIdent](events::EIntegerCompleteEvent event){
        shared::SetValue<i32>(fSelectedEntity, nameIdent, event.Value);
    });

    shared::Events().AddEntityChangeEventListener(nameIdent, [this, weakResult](ERegister::Entity entity, const EString& valueIdent){
        if (entity != fSelectedEntity) { return; }
        if (weakResult.expired()) { return; }
        ERef<EProperty> prop = shared::GetValue(entity, valueIdent);
        weakResult.lock()->SetValue(std::static_pointer_cast<EValueProperty<u32>>(prop)->GetValue());
    }, result.get());
    return result;
}

ERef<EUIField> EObjectView::RenderInteger(Engine::EValueProperty<u64>* storage, EString nameIdent) 
{
    ERef<EUIIntegerEdit> result = EMakeRef<EUIIntegerEdit>(storage->GetPropertyName().c_str());
    EWeakRef<EUIIntegerEdit> weakResult = result;
    result->SetValue(storage->GetValue());

    result->AddEventListener<events::EIntegerCompleteEvent>([this, nameIdent](events::EIntegerCompleteEvent event){
        shared::SetValue<i32>(fSelectedEntity, nameIdent, event.Value);
    });


    shared::Events().AddEntityChangeEventListener(nameIdent, [this, weakResult](ERegister::Entity entity, const EString& valueIdent){
        if (entity != fSelectedEntity) { return; }
        if (weakResult.expired()) { return; }
        ERef<EProperty> prop = shared::GetValue(entity, valueIdent);
        weakResult.lock()->SetValue(std::static_pointer_cast<EValueProperty<u64>>(prop)->GetValue());
    }, result.get());
    return result;
}

ERef<EUIField> EObjectView::RenderDouble(Engine::EValueProperty<double>* storage, EString nameIdent) 
{
    ERef<EUIFloatEdit> result = EMakeRef<EUIFloatEdit>(storage->GetPropertyName().c_str());
    EWeakRef<EUIFloatEdit> weakResult = result;

    result->SetValue(storage->GetValue());

    result->AddEventListener<events::EFloatCompleteEvent>([this, nameIdent](events::EFloatCompleteEvent event){
        shared::SetValue<double>(fSelectedEntity, nameIdent, event.Value);
    });


    shared::Events().AddEntityChangeEventListener(nameIdent, [this, weakResult](ERegister::Entity entity, const EString& valueIdent){
        if (entity != fSelectedEntity) { return; }
        if (weakResult.expired()) { return; }
        ERef<EProperty> prop = shared::GetValue(entity, valueIdent);
        weakResult.lock()->SetValue((float)std::static_pointer_cast<EValueProperty<double>>(prop)->GetValue());
    }, result.get());
    return result;
}

ERef<EUIField> EObjectView::RenderDouble(Engine::EValueProperty<float>* storage, EString nameIdent) 
{
    ERef<EUIFloatEdit> result = EMakeRef<EUIFloatEdit>(storage->GetPropertyName().c_str());
    EWeakRef<EUIFloatEdit> weakResult = result;

    result->SetValue(storage->GetValue());

    result->AddEventListener<events::EFloatCompleteEvent>([this, nameIdent](events::EFloatCompleteEvent event){
        shared::SetValue<float>(fSelectedEntity, nameIdent, event.Value);
    });


    shared::Events().AddEntityChangeEventListener(nameIdent, [this, weakResult](ERegister::Entity entity, const EString& valueIdent){
        if (entity != fSelectedEntity) { return; }
        if (weakResult.expired()) { return; }
        ERef<EProperty> prop = shared::GetValue(entity, valueIdent);
        weakResult.lock()->SetValue((float)std::static_pointer_cast<EValueProperty<float>>(prop)->GetValue());
    }, result.get());
    return result;
}

ERef<EUIField> EObjectView::RenderString(Engine::EValueProperty<EString>* storage, EString nameIdent) 
{
    ERef<EUITextField> result = EMakeRef<EUITextField>(storage->GetPropertyName().c_str());
    EWeakRef<EUITextField> weakResult = result;
    result->SetValue(storage->GetValue());

    result->AddEventListener<events::ETextCompleteEvent>([this, nameIdent](events::ETextCompleteEvent event){
        shared::SetValue<EString>(fSelectedEntity, nameIdent, event.Value);
    });

    shared::Events().AddEntityChangeEventListener(nameIdent, [weakResult](ERegister::Entity entity, const EString& valueIdent){
        if (weakResult.expired()) { return; }
        ERef<EProperty> prop = shared::GetValue(entity, valueIdent);
        weakResult.lock()->SetValue(std::static_pointer_cast<EValueProperty<EString>>(prop)->GetValue());
    }, result.get());
    return result;
}

void EObjectView::RegenComponentsView() 
{
    if (fComponentsView.expired()) { return; }
    fComponentsView.lock()->Clear();

    if (fSelectedEntity == 0) { return; }
    
    EVector<ERef<EProperty>> allComponents = shared::GetAllComponents(fSelectedEntity);
    for (auto component : allComponents)
    {
        fComponentsView.lock()->AddChild(RenderProperty(component.get(), component->GetPropertyName()));
    }
}

void EObjectView::RegenAddComponentMenu() 
{
    ERef<EUIField> componentContextMenu = EMakeRef<EUIField>("Context Menu");
    ERef<EUIMenu> addMenu = EMakeRef<EUIMenu>("Add Component");

    EVector<EComponentRegisterEntry> allComponents = shared::ExtensionManager().GetComponentRegister().GetAllItems();
    for (const EComponentRegisterEntry& dsc : allComponents)
    {
        EWeakRef<EUIField> item = addMenu->AddChild(EMakeRef<EUIMenuItem>(dsc.Description.GetId()));
        item.lock()->AddEventListener<events::EButtonEvent>([this, dsc](){
            if (dsc.DefaultValue)
            {
                shared::CreateComponent(dsc.DefaultValue.get(), fSelectedEntity);
            }
            else
            {
                shared::CreateComponent(dsc.Description, fSelectedEntity);
            }
        });
    }

    componentContextMenu->AddChild(addMenu);
    fComponentsView.lock()->SetContextMenu(componentContextMenu);
}
