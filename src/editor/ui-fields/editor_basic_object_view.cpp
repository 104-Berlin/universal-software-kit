#include "editor.h"

using namespace Editor;
using namespace Engine;

EObjectView::EObjectView()
    : EUIField("OBJECTVIEW"), fSelectedEntity(0)
{
    
    shared::StaticSharedContext::instance().Events().GetEventDispatcher().Connect<EntityCreateEvent>([this](EntityCreateEvent event){
        fEntities.push_back(event.Handle);
        if (!fEntitiesTable.expired())
        {
            ERef<EUITableRow> newRow = EMakeRef<EUITableRow>();
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

            newRow->AddChild(EMakeRef<EUILabel>("Some Text"));
            fEntitiesTable.lock()->AddChild(newRow);
            fEntityRows.insert({event.Handle, newRow});
        }
    });
    shared::StaticSharedContext::instance().Events().GetEventDispatcher().Connect<ComponentCreateEvent>([this](ComponentCreateEvent event){
        if (event.Handle == fSelectedEntity)
        {
            std::lock_guard<std::mutex> lk(fChangeComponentsMtx);
            ERef<EProperty> newComponent = shared::GetValue(event.Handle, event.ValueId);
            if (newComponent)
            {
                fSelectedComponents.push_back(newComponent);
            }
        }
    });
    shared::StaticSharedContext::instance().Events().GetEventDispatcher().Connect<ValueChangeEvent>([this](ValueChangeEvent event){
        if (event.Handle == fSelectedEntity)
        {
            EVector<EString> idents = EStringUtil::SplitString(event.Identifier, ".");
            EVector<ERef<EProperty>>::iterator it = std::find_if(fSelectedComponents.begin(), fSelectedComponents.end(), [idents](ERef<EProperty> prop){return prop->GetDescription().GetId() == idents[0];});
            if (it != fSelectedComponents.end())
            {
                ERef<EProperty> newValue = shared::GetValue(fSelectedEntity, event.Identifier);
                if (idents.size() == 1)
                {
                    (*it)->Copy(newValue.get());
                }          
                else if ((*it)->GetDescription().GetType() == EValueType::STRUCT)
                {
                    EProperty* copyToProp = std::dynamic_pointer_cast<EStructProperty>((*it))->GetPropertyByIdentifier(event.Identifier.substr(idents[0].length() + 1));
                    if (copyToProp)
                    {
                        copyToProp->Copy(newValue.get());
                    }
                }      
            }
        }
    });

    ERef<EUIContainer> entitiesList = EMakeRef<EUIContainer>("Entities");
    entitiesList->SetWidth(100);
    EWeakRef<EUIField> addObjectButton = entitiesList->AddChild(EMakeRef<EUIButton>("Add Object"));
    addObjectButton.lock()->AddEventListener<events::EButtonEvent>(&shared::CreateEntity);

    fEntitiesTable = std::dynamic_pointer_cast<EUITable>(entitiesList->AddChild(EMakeRef<EUITable>()).lock());
    fEntitiesTable.lock()->SetHeaderCells({
        "ID",
        "More Info"
    });



    ERef<EUIContainer> componentsList = EMakeRef<EUIContainer>("Components");
    componentsList->SetCustomUpdateFunction([this](){RegenComponentsView();});

    fComponentsView = componentsList;

    AddChild(entitiesList);
    AddChild(EMakeRef<EUISameLine>());
    AddChild(componentsList);
}

bool EObjectView::OnRender() 
{
    return true;
}

void EObjectView::OnUpdateEventDispatcher()
{
}

ERef<EUIField> EObjectView::RenderProperty(Engine::EProperty* storage, EString nameIdent) 
{
    EValueDescription propertyDsc = storage->GetDescription();
    EValueType type = propertyDsc.GetType();

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
    EVector<EProperty*> elements = storage->GetElements();
    ERef<EUIField> field = EMakeRef<EUIField>("Some Label");
    
    for (size_t i = 0; i < elements.size(); i++)
    {
        EProperty* element = elements[i];
        field->AddChild(RenderProperty(element, nameIdent + "." + std::to_string(i)));
    }

    EWeakRef<EUIField> addElemButton = field->AddChild(EMakeRef<EUIButton>("Add Element"));
    addElemButton.lock()->AddEventListener<events::EButtonEvent>([this, nameIdent](){
        shared::AddArrayEntry(fSelectedEntity, nameIdent);
    });
    return field;
}

ERef<EUIField> EObjectView::RenderBool(Engine::EValueProperty<bool>* storage, EString nameIdent) 
{
    ERef<EUICheckbox> result = EMakeRef<EUICheckbox>(storage->GetPropertyName().c_str());

    result->AddEventListener<events::ECheckboxEvent>([this, nameIdent](events::ECheckboxEvent event){
        shared::SetValue<bool>(fSelectedEntity, nameIdent, event.Checked);
    });
    return result;
}

ERef<EUIField> EObjectView::RenderInteger(Engine::EValueProperty<i32>* storage, EString nameIdent) 
{
    ERef<EUIIntegerEdit> result = EMakeRef<EUIIntegerEdit>(storage->GetPropertyName().c_str());

    result->AddEventListener<events::EIntegerCompleteEvent>([this, nameIdent](events::EIntegerCompleteEvent event){
        shared::SetValue<i32>(fSelectedEntity, nameIdent, event.Value);
    });
    return result;
}

ERef<EUIField> EObjectView::RenderInteger(Engine::EValueProperty<u32>* storage, EString nameIdent) 
{
    ERef<EUIIntegerEdit> result = EMakeRef<EUIIntegerEdit>(storage->GetPropertyName().c_str());

    result->AddEventListener<events::EIntegerCompleteEvent>([this, nameIdent](events::EIntegerCompleteEvent event){
        shared::SetValue<i32>(fSelectedEntity, nameIdent, event.Value);
    });
    return result;
}

ERef<EUIField> EObjectView::RenderInteger(Engine::EValueProperty<u64>* storage, EString nameIdent) 
{
    ERef<EUIIntegerEdit> result = EMakeRef<EUIIntegerEdit>(storage->GetPropertyName().c_str());

    result->AddEventListener<events::EIntegerCompleteEvent>([this, nameIdent](events::EIntegerCompleteEvent event){
        shared::SetValue<i32>(fSelectedEntity, nameIdent, event.Value);
    });
    return result;
}

ERef<EUIField> EObjectView::RenderDouble(Engine::EValueProperty<double>* storage, EString nameIdent) 
{
    ERef<EUIFloatEdit> result = EMakeRef<EUIFloatEdit>(storage->GetPropertyName().c_str());

    result->AddEventListener<events::EFloatCompleteEvent>([this, nameIdent](events::EFloatCompleteEvent event){
        shared::SetValue<float>(fSelectedEntity, nameIdent, event.Value);
    });
    return result;
}

ERef<EUIField> EObjectView::RenderString(Engine::EValueProperty<EString>* storage, EString nameIdent) 
{
    ERef<EUITextField> result = EMakeRef<EUITextField>(storage->GetPropertyName().c_str());

    result->AddEventListener<events::ETextCompleteEvent>([this, nameIdent](events::ETextCompleteEvent event){
        shared::SetValue<EString>(fSelectedEntity, nameIdent, event.Value);
    });
    return result;
}

bool EObjectView::HasSelectedComponent(Engine::EValueDescription dsc) 
{
    return std::find_if(fSelectedComponents.begin(), fSelectedComponents.end(), [dsc](ERef<Engine::EProperty> property){
        return property->GetDescription() == dsc;
    }) != fSelectedComponents.end();
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
