#include "editor.h"

using namespace Editor;
using namespace Engine;

EObjectView::EObjectView(Engine::EUIValueRegister* valueFieldRegister)
    : EUIField("OBJECTVIEW"), fSelectedEntity(0), fUIValueRegister(valueFieldRegister)
{
    shared::Events().Connect<EntityChangeEvent>([this](EntityChangeEvent e){
        if (e.Type == EntityChangeType::COMPONENT_ADDED && e.Entity.Handle == fSelectedEntity)
        {
            fComponentsView.lock()->SetDirty();
        }
        else if (e.Type == EntityChangeType::ENTITY_CREATED)
        {
            if (fEntitiesTable.expired()) { return; }
    
            ERef<EUITableRow> newRow = EMakeRef<EUITableRow>();
            fEntityRows.insert({e.Entity.Handle, newRow});
            fEntitiesTable.lock()->AddChild(newRow);

            EWeakRef<EUISelectable> selectabe = std::dynamic_pointer_cast<EUISelectable>(newRow->AddChild(EMakeRef<EUISelectable>(std::to_string(e.Entity.Handle))).lock());
            selectabe.lock()->SetStretchToAllColumns(true);

            selectabe.lock()->AddEventListener<events::ESelectableChangeEvent>([this, e](events::ESelectableChangeEvent se){
                if (se.Selected)
                {
                    if (fSelectedEntity)
                    {
                        std::dynamic_pointer_cast<EUISelectable>(fEntityRows[fSelectedEntity].lock()->GetChildAt(0).lock())->SetSelected(false);
                    }

                    fSelectedEntity = e.Entity.Handle;
                }
                else
                {
                    fSelectedEntity = 0;
                }
                fComponentsView.lock()->SetDirty();
            });
        }
        
    }, this);

    shared::Events().Connect<events::EExtensionLoadedEvent>([this](events::EExtensionLoadedEvent e){
        fComponentsView.lock()->GetContextMenu().lock()->SetDirty();
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
        return valueRender.second(storage, nameIdent, [this, nameIdent](EProperty* property){
            shared::SetValue(fSelectedEntity, nameIdent, property);
        });
    }

    switch (type)
    {
    case EValueType::STRUCT: return RenderStruct(static_cast<EStructProperty*>(storage), nameIdent); break;
    case EValueType::ARRAY: return RenderArray(static_cast<EArrayProperty*>(storage), nameIdent); break;
    case EValueType::PRIMITIVE: return RenderPrimitive(storage, nameIdent); break;
    case EValueType::ENUM: return RenderEnum(static_cast<EEnumProperty*>(storage), nameIdent); break;
    case EValueType::ANY:
    {
        EAny value;
        if (static_cast<EStructProperty*>(storage)->GetValue(value))
        {
            //return RenderProperty(value.Value(), nameIdent);
        }
        break;
    }
    case EValueType::UNKNOWN: break;
    }
    return nullptr;
}

ERef<EUIField> EObjectView::RenderStruct(EStructProperty* storage, EString nameIdent) 
{
    ERef<EUIField> result = EMakeRef<EUIField>("STRUCT");
    EValueDescription description = storage->GetDescription();
    EVector<Engine::EValueDescription::StructField> structFields = description.GetStructFields();
    for (size_t i = 0; i < structFields.size(); i++)
    {
        auto& entry = structFields[i];
        const EString& propertyName = entry.first;
        result->AddChild(RenderProperty(storage->GetProperty(propertyName), nameIdent + "." + propertyName));
        if (i < structFields.size() - 1)
        {
            result->AddChild(EMakeRef<EUIDivider>());
        }
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
    EValueDescription description = storage->GetDescription();
    EVector<EString> enumValues = description.GetEnumOptions();

    ERef<EUIDropdown> result = EMakeRef<EUIDropdown>(storage->GetPropertyName());
    EWeakRef<EUIDropdown> weakResult = result;
    result->SetOptions(enumValues);
    result->SetSelectedIndex(storage->GetCurrentValue());
    result->AddEventListener<events::ESelectChangeEvent>([this, nameIdent](events::ESelectChangeEvent event){
        shared::SetEnumValue(fSelectedEntity, nameIdent, event.Index);
    });

    shared::Events().Connect<EntityChangeEvent>([this, weakResult, nameIdent](EntityChangeEvent e){
        if (e.Entity.Handle != fSelectedEntity) { return; }
        if (weakResult.expired()) { return; }
        if (e.Type == EntityChangeType::COMPONENT_CHANGED && e.Data.Value())
        {
            ComponentChangeData componentChangeData;
            if (convert::getter(e.Data.Value(), &componentChangeData))
            {
                if (componentChangeData.NewValue.Value())
                {
                    EProperty* valueProp = static_cast<EStructProperty*>(componentChangeData.NewValue.Value())->GetPropertyByIdentifier(nameIdent);
                    if (valueProp)
                    {
                        weakResult.lock()->SetSelectedIndex(static_cast<EEnumProperty*>(valueProp)->GetCurrentValue());
                    }
                }
            }
        }
    }, result.get());

    return result;
}

ERef<EUIField> EObjectView::RenderArray(Engine::EArrayProperty* storage, EString nameIdent) 
{
    ERef<EUIField> field = EMakeRef<EUIField>("Some Label");
    EWeakRef<EUIField> weakRef = field;

    field->SetDirty();
    field->SetCustomUpdateFunction([this, weakRef, nameIdent](){
        ERef<EProperty> foundProp = shared::GetValueFromIdent(fSelectedEntity, nameIdent);
        if (foundProp->GetDescription().GetType() != EValueType::ARRAY) { return; }
        weakRef.lock()->Clear();

        
        Engine::EArrayProperty* storage = (EArrayProperty*)foundProp.get();
        EVector<EProperty*> elements = storage->GetElements();

        for (size_t i = 0; i < elements.size(); i++)
        {
            EProperty* element = elements[i];

            EWeakRef<EUIField> groupPanel = weakRef.lock()->AddChild(EMakeRef<EUIGroupPanel>(element->GetPropertyName()));
            groupPanel.lock()->AddChild(RenderProperty(element, nameIdent + "." + std::to_string(i)));
        }

        EWeakRef<EUIField> addElemButton = weakRef.lock()->AddChild(EMakeRef<EUIButton>("Add Element"));
        addElemButton.lock()->AddEventListener<events::EButtonEvent>([this, nameIdent](){
            shared::AddArrayEntry(fSelectedEntity, nameIdent);
        });
    });


    shared::Events().Connect<EntityChangeEvent>([this, weakRef, nameIdent](EntityChangeEvent e){
        if (e.Entity.Handle != fSelectedEntity) { return; }
        if (weakRef.expired()) { return; }
        if (e.Type == EntityChangeType::COMPONENT_CHANGED && e.Data.Value())
        {
            ComponentChangeData componentChangeData;
            if (convert::getter(e.Data.Value(), &componentChangeData))
            {
                if (componentChangeData.Identifier == nameIdent)
                {
                    weakRef.lock()->SetDirty();
                }
            }
        }
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

    shared::Events().Connect<EntityChangeEvent>([this, weakResult, nameIdent](EntityChangeEvent e){
        if (e.Entity.Handle != fSelectedEntity) { return; }
        if (weakResult.expired()) { return; }
        if (e.Type == EntityChangeType::COMPONENT_CHANGED && e.Data.Value())
        {
            ComponentChangeData componentChangeData;
            if (convert::getter(e.Data.Value(), &componentChangeData))
            {
                if (componentChangeData.NewValue.Value())
                {
                    EProperty* valueProp = static_cast<EStructProperty*>(componentChangeData.NewValue.Value())->GetPropertyByIdentifier(nameIdent);
                    if (valueProp)
                    {
                        weakResult.lock()->SetValue(static_cast<EValueProperty<bool>*>(valueProp)->GetValue());
                    }
                }
            }
        }
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

    shared::Events().Connect<EntityChangeEvent>([this, weakResult, nameIdent](EntityChangeEvent e){
        if (e.Entity.Handle != fSelectedEntity) { return; }
        if (weakResult.expired()) { return; }
        if (e.Type == EntityChangeType::COMPONENT_CHANGED && e.Data.Value())
        {
            ComponentChangeData componentChangeData;
            if (convert::getter(e.Data.Value(), &componentChangeData))
            {
                if (componentChangeData.NewValue.Value())
                {
                    EProperty* valueProp = static_cast<EStructProperty*>(componentChangeData.NewValue.Value())->GetPropertyByIdentifier(nameIdent);
                    if (valueProp)
                    {
                        weakResult.lock()->SetValue(static_cast<EValueProperty<i32>*>(valueProp)->GetValue());
                    }
                }
            }
        }
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

    shared::Events().Connect<EntityChangeEvent>([this, weakResult, nameIdent](EntityChangeEvent e){
        if (e.Entity.Handle != fSelectedEntity) { return; }
        if (weakResult.expired()) { return; }
        if (e.Type == EntityChangeType::COMPONENT_CHANGED && e.Data.Value())
        {
            ComponentChangeData componentChangeData;
            if (convert::getter(e.Data.Value(), &componentChangeData))
            {
                if (componentChangeData.NewValue.Value())
                {
                    EProperty* valueProp = static_cast<EStructProperty*>(componentChangeData.NewValue.Value())->GetPropertyByIdentifier(nameIdent);
                    if (valueProp)
                    {
                        weakResult.lock()->SetValue(static_cast<EValueProperty<u32>*>(valueProp)->GetValue());
                    }
                }
            }
        }
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

    shared::Events().Connect<EntityChangeEvent>([this, weakResult, nameIdent](EntityChangeEvent e){
        if (e.Entity.Handle != fSelectedEntity) { return; }
        if (weakResult.expired()) { return; }
        if (e.Type == EntityChangeType::COMPONENT_CHANGED && e.Data.Value())
        {
            ComponentChangeData componentChangeData;
            if (convert::getter(e.Data.Value(), &componentChangeData))
            {
                if (componentChangeData.NewValue.Value())
                {
                    EProperty* valueProp = static_cast<EStructProperty*>(componentChangeData.NewValue.Value())->GetPropertyByIdentifier(nameIdent);
                    if (valueProp)
                    {
                        weakResult.lock()->SetValue(static_cast<EValueProperty<u64>*>(valueProp)->GetValue());
                    }
                }
            }
        }
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

    shared::Events().Connect<EntityChangeEvent>([this, weakResult, nameIdent](EntityChangeEvent e){
        if (e.Entity.Handle != fSelectedEntity) { return; }
        if (weakResult.expired()) { return; }
        if (e.Type == EntityChangeType::COMPONENT_CHANGED && e.Data.Value())
        {
            ComponentChangeData componentChangeData;
            if (convert::getter(e.Data.Value(), &componentChangeData))
            {
                if (componentChangeData.NewValue.Value())
                {
                    EProperty* valueProp = static_cast<EStructProperty*>(componentChangeData.NewValue.Value())->GetPropertyByIdentifier(nameIdent);
                    if (valueProp)
                    {
                        weakResult.lock()->SetValue(static_cast<EValueProperty<double>*>(valueProp)->GetValue());
                    }
                }
            }
        }
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

    shared::Events().Connect<EntityChangeEvent>([this, weakResult, nameIdent](EntityChangeEvent e){
        if (e.Entity.Handle != fSelectedEntity) { return; }
        if (weakResult.expired()) { return; }
        if (e.Type == EntityChangeType::COMPONENT_CHANGED && e.Data.Value())
        {
            ComponentChangeData componentChangeData;
            if (convert::getter(e.Data.Value(), &componentChangeData))
            {
                if (componentChangeData.NewValue.Value())
                {
                    EProperty* valueProp = static_cast<EStructProperty*>(componentChangeData.NewValue.Value())->GetPropertyByIdentifier(nameIdent);
                    if (valueProp)
                    {
                        weakResult.lock()->SetValue(static_cast<EValueProperty<float>*>(valueProp)->GetValue());
                    }
                }
            }
        }
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

    shared::Events().Connect<EntityChangeEvent>([this, weakResult, nameIdent](EntityChangeEvent e){
        if (e.Entity.Handle != fSelectedEntity) { return; }
        if (weakResult.expired()) { return; }
        if (e.Type == EntityChangeType::COMPONENT_CHANGED && e.Data.Value())
        {
            ComponentChangeData componentChangeData;
            if (convert::getter(e.Data.Value(), &componentChangeData))
            {
                if (componentChangeData.NewValue.Value())
                {
                    EProperty* valueProp = static_cast<EStructProperty*>(componentChangeData.NewValue.Value())->GetPropertyByIdentifier(nameIdent);
                    if (valueProp)
                    {
                        weakResult.lock()->SetValue(static_cast<EValueProperty<EString>*>(valueProp)->GetValue());
                    }
                }
            }
        }
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
        EWeakRef<EUIField> collapsable = fComponentsView.lock()->AddChild(EMakeRef<EUICollapsable>(component->GetPropertyName()));
        collapsable.lock()->AddChild(RenderProperty(component.get(), component->GetPropertyName()));
    }
}

void EObjectView::RegenAddComponentMenu() 
{
    ERef<EUIField> componentContextMenu = EMakeRef<EUIField>("Context Menu");
    EWeakRef<EUIField> componentWeakRefMenu = componentContextMenu;
    componentContextMenu->SetDirty();
    componentContextMenu->SetCustomUpdateFunction([this, componentWeakRefMenu](){
        if (componentWeakRefMenu.expired()) { return; }
        componentWeakRefMenu.lock()->Clear();
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

        componentWeakRefMenu.lock()->AddChild(addMenu);
    });
    fComponentsView.lock()->SetContextMenu(componentContextMenu);
}


/***
 * 
 * COMPONENT EDIT
 * 
 ****/

EComponentEdit::EComponentEdit(const ERef<EProperty>& property)
    : EUIField("Component Edit"), fEditedProperty(property)
{
    AddChild(RenderProperty(fEditedProperty.get(), fEditedProperty->GetPropertyName()));
}

ERef<EUIField> EComponentEdit::RenderProperty(Engine::EProperty* storage, EString nameIdent) 
{
    EValueDescription propertyDsc = storage->GetDescription();
    EValueType type = propertyDsc.GetType();

    EUIValueRegisterEntry valueRender;
    if (EApplication::gApp()->GetUIValueRegister().FindItem(EFindValueFieldByType(propertyDsc.GetId()), &valueRender))
    {
        return valueRender.second(storage, nameIdent, [this, nameIdent](Engine::EProperty* storage){
            // We use post because this is called in event loop
            fEventDispatcher.Post<events::ComponentEditChangeEvent>({nameIdent});
        });
    }

    switch (type)
    {
    case EValueType::STRUCT: return RenderStruct(static_cast<EStructProperty*>(storage), nameIdent);
    case EValueType::ARRAY: return RenderArray(static_cast<EArrayProperty*>(storage), nameIdent); 
    case EValueType::PRIMITIVE: return RenderPrimitive(storage, nameIdent);
    case EValueType::ENUM: return RenderEnum(static_cast<EEnumProperty*>(storage), nameIdent); 
    case EValueType::ANY: return RenderAny(static_cast<EStructProperty*>(storage), nameIdent); 
    case EValueType::UNKNOWN: break;
    }
    return nullptr;
}

ERef<EUIField> EComponentEdit::RenderStruct(Engine::EStructProperty* storage, EString nameIdent) 
{
    ERef<EUIField> result = EMakeRef<EUIField>("STRUCT");
    EValueDescription description = storage->GetDescription();
    EVector<Engine::EValueDescription::StructField> structFields = description.GetStructFields();
    for (size_t i = 0; i < structFields.size(); i++)
    {
        auto& entry = structFields[i];
        const EString& propertyName = entry.first;
        result->AddChild(RenderProperty(storage->GetProperty(propertyName), nameIdent + "." + propertyName));
        if (i < structFields.size() - 1)
        {
            result->AddChild(EMakeRef<EUIDivider>());
        }
    }
    return result;
}

ERef<EUIField> EComponentEdit::RenderPrimitive(Engine::EProperty* storage, EString nameIdent) 
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

ERef<EUIField> EComponentEdit::RenderEnum(Engine::EEnumProperty* storage, EString nameIdent) 
{
    EValueDescription description = storage->GetDescription();
    EVector<EString> enumValues = description.GetEnumOptions();

    ERef<EUIDropdown> result = EMakeRef<EUIDropdown>(storage->GetPropertyName());
    EWeakRef<EUIDropdown> weakResult = result;
    result->SetOptions(enumValues);
    result->SetSelectedIndex(storage->GetCurrentValue());
    result->AddEventListener<events::ESelectChangeEvent>([this, nameIdent, storage](events::ESelectChangeEvent event){
        storage->SetCurrentValue(event.Index);
        fEventDispatcher.Enqueue<events::ComponentEditChangeEvent>({nameIdent});
    });

    return result;
}

ERef<EUIField> EComponentEdit::RenderArray(Engine::EArrayProperty* storage, EString nameIdent) 
{
    ERef<EUIField> field = EMakeRef<EUIField>("Some Label");
    EWeakRef<EUIField> weakRef = field;

    field->SetDirty();
    field->SetCustomUpdateFunction([this, weakRef, storage, nameIdent](){
        weakRef.lock()->Clear();
        EVector<EProperty*> elements = storage->GetElements();

        for (size_t i = 0; i < elements.size(); i++)
        {
            EProperty* element = elements[i];

            EWeakRef<EUIField> groupPanel = weakRef.lock()->AddChild(EMakeRef<EUIGroupPanel>(element->GetPropertyName()));
            groupPanel.lock()->AddChild(RenderProperty(element, nameIdent + "." + std::to_string(i)));
        }

        EWeakRef<EUIField> addElemButton = weakRef.lock()->AddChild(EMakeRef<EUIButton>("Add Element"));
        addElemButton.lock()->AddEventListener<events::EButtonEvent>([this, storage, weakRef](){
            storage->AddElement();
            weakRef.lock()->SetDirty();
        });
    });
    return field;
}

ERef<EUIField> EComponentEdit::RenderAny(Engine::EStructProperty* storage, EString nameIdent) 
{
    ERef<EUIField> result = EMakeRef<EUIField>("ANY");
    ERef<EUIDropdown> dropDown = EMakeRef<EUIDropdown>("Type");
    ERef<EUIGroupPanel> groupPanel = EMakeRef<EUIGroupPanel>("Value");
    EWeakRef<EUIGroupPanel> weakGroupPanel = groupPanel;

    
    result->AddChild(dropDown);
    result->AddChild(groupPanel);

    // Set up dropdown
    dropDown->AddOption("None");
    for (auto& entry : shared::ExtensionManager().GetComponentRegister().GetAllItems())
    {
        dropDown->AddOption(entry.Description.GetId());
    }
    


    dropDown->AddEventListener<events::ESelectChangeEvent>([storage, weakGroupPanel](events::ESelectChangeEvent e){
        EAny newValue;
        if (e.Index != 0)
        {
            EComponentRegisterEntry newValueDsc;
            if (shared::ExtensionManager().GetComponentRegister().FindItem(EFindTypeDescByName(e.Option), &newValueDsc))
            {
                ERef<EProperty> newValueProp = ERef<EProperty>(EProperty::CreateFromDescription("value", newValueDsc.Description));
                newValueProp->Copy(newValueDsc.DefaultValue.get());
                newValue.SetValue(newValueProp);
            }
        }
        storage->SetValue<EAny>(newValue);
        if (!weakGroupPanel.expired())
        {
            weakGroupPanel.lock()->SetDirty();
        }
    });

    // Set up value display
    groupPanel->SetCustomUpdateFunction([this, weakGroupPanel, storage, nameIdent](){
        if (weakGroupPanel.expired()) { return; }

        weakGroupPanel.lock()->Clear();
        
        EStructProperty* structProp = static_cast<EStructProperty*>(storage);
        EProperty* valueProp = structProp->GetProperty("value");
        if (valueProp)
        {
            ERef<EUIField> propertyField = this->RenderProperty(valueProp, nameIdent + ".value");
            weakGroupPanel.lock()->AddChild(propertyField);
        }
    });
    return result;
}

ERef<EUIField> EComponentEdit::RenderBool(Engine::EValueProperty<bool>* storage, EString nameIdent) 
{
    ERef<EUICheckbox> result = EMakeRef<EUICheckbox>(storage->GetPropertyName().c_str());

    result->SetValue(storage->GetValue());

    result->AddEventListener<events::ECheckboxEvent>([this, nameIdent, storage](events::ECheckboxEvent event){
        storage->SetValue(event.Checked);
        fEventDispatcher.Enqueue<events::ComponentEditChangeEvent>({nameIdent});
    });
    return result;
}

ERef<EUIField> EComponentEdit::RenderInteger(Engine::EValueProperty<i32>* storage, EString nameIdent) 
{
    ERef<EUIIntegerEdit> result = EMakeRef<EUIIntegerEdit>(storage->GetPropertyName().c_str());

    result->SetValue(storage->GetValue());

    result->AddEventListener<events::EIntegerCompleteEvent>([this, nameIdent, storage](events::EIntegerCompleteEvent event){
        storage->SetValue(event.Value);
        fEventDispatcher.Enqueue<events::ComponentEditChangeEvent>({nameIdent});
    });

    return result;
}

ERef<EUIField> EComponentEdit::RenderInteger(Engine::EValueProperty<u32>* storage, EString nameIdent) 
{
    ERef<EUIIntegerEdit> result = EMakeRef<EUIIntegerEdit>(storage->GetPropertyName().c_str());

    result->SetValue(storage->GetValue());

    result->AddEventListener<events::EIntegerCompleteEvent>([this, nameIdent, storage](events::EIntegerCompleteEvent event){
        storage->SetValue(event.Value);
        fEventDispatcher.Enqueue<events::ComponentEditChangeEvent>({nameIdent});
    });

    return result;
}

ERef<EUIField> EComponentEdit::RenderInteger(Engine::EValueProperty<u64>* storage, EString nameIdent) 
{
    ERef<EUIIntegerEdit> result = EMakeRef<EUIIntegerEdit>(storage->GetPropertyName().c_str());
    result->SetValue(storage->GetValue());

    result->AddEventListener<events::EIntegerCompleteEvent>([this, nameIdent, storage](events::EIntegerCompleteEvent event){
        storage->SetValue(event.Value);
        fEventDispatcher.Enqueue<events::ComponentEditChangeEvent>({nameIdent});
    });
    return result;
}

ERef<EUIField> EComponentEdit::RenderDouble(Engine::EValueProperty<double>* storage, EString nameIdent) 
{
    ERef<EUIFloatEdit> result = EMakeRef<EUIFloatEdit>(storage->GetPropertyName().c_str());

    result->SetValue(storage->GetValue());

    result->AddEventListener<events::EFloatCompleteEvent>([this, nameIdent, storage](events::EFloatCompleteEvent event){
        storage->SetValue(event.Value);
        fEventDispatcher.Enqueue<events::ComponentEditChangeEvent>({nameIdent});
    });

    return result;
}

ERef<EUIField> EComponentEdit::RenderDouble(Engine::EValueProperty<float>* storage, EString nameIdent) 
{
    ERef<EUIFloatEdit> result = EMakeRef<EUIFloatEdit>(storage->GetPropertyName().c_str());

    result->SetValue(storage->GetValue());

    result->AddEventListener<events::EFloatCompleteEvent>([this, nameIdent, storage](events::EFloatCompleteEvent event){
        storage->SetValue(event.Value);
        this->fEventDispatcher.Enqueue<events::ComponentEditChangeEvent>({nameIdent});
    });
    return result;
}

ERef<EUIField> EComponentEdit::RenderString(Engine::EValueProperty<EString>* storage, EString nameIdent) 
{
    ERef<EUITextField> result = EMakeRef<EUITextField>(storage->GetPropertyName().c_str());
    EWeakRef<EUITextField> weakResult = result;
    result->SetValue(storage->GetValue());

    result->AddEventListener<events::ETextCompleteEvent>([this, nameIdent, storage](events::ETextCompleteEvent event){
        storage->SetValue(event.Value);
        this->fEventDispatcher.Enqueue<events::ComponentEditChangeEvent>({nameIdent});
    });
    return result;
}
