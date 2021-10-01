#include "editor.h"

using namespace Editor;
using namespace Engine;

EObjectView::EObjectView()
    : EUIField("OBJECTVIEW"), fSelectedEntity(0)
{
    fAddObjectButton = EMakeRef<EUIButton>("Add Object");
    fAddObjectButton->AddEventListener<events::EButtonEvent>([this](){
        shared::CreateEntity();
    });
    shared::StaticSharedContext::instance().Events().GetEventDispatcher().Connect<EntityCreateEvent>([this](EntityCreateEvent event){
        fEntities.push_back(event.Handle);
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
}

bool EObjectView::OnRender() 
{
    ImGui::BeginChild("Entity Child", {100, 0});
    for (ERegister::Entity entity : fEntities)
    {
        EString entityIdent = "Entity " + std::to_string(entity);
        bool selected = fSelectedEntity == entity;
        if (ImGui::Selectable(entityIdent.c_str(), &selected))
        {
            fSelectedEntity = fSelectedEntity == entity ? 0 : entity;
            fSelectedComponents.clear();
            if (fSelectedEntity)
            {
                fSelectedComponents = shared::GetAllComponents(fSelectedEntity);
            }
        }
    }
    fAddObjectButton->Render();
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("ComponentChild");
    if (fSelectedEntity)
    {
        {
            std::lock_guard<std::mutex> lk(fChangeComponentsMtx);
            //E_INFO("Rendering components: " + std::to_string(fSelectedComponents.size()));
            for (ERef<EProperty> storage : fSelectedComponents)
            {
                RenderProperty(storage.get(), storage->GetPropertyName());
            }
        }

        if (ImGui::Button("Add Component"))
        {
            ImGui::OpenPopup("add-component-popup");
        }
        if (ImGui::BeginPopup("add-component-popup"))
        {
            for (EValueDescription compDsc : shared::StaticSharedContext::instance().GetExtensionManager().GetTypeRegister().GetAllItems())
            {
                if (compDsc.GetType() != EValueType::STRUCT) { continue; }
                bool hasComp = HasSelectedComponent(compDsc);
                if (!hasComp)
                {
                    if (ImGui::Selectable(compDsc.GetId().c_str()))
                    {
                        shared::CreateComponent(compDsc, fSelectedEntity);
                    }
                }
            }
            ImGui::EndPopup();
        }
    }
    ImGui::EndChild();


    return true;
}

void EObjectView::OnUpdateEventDispatcher()
{
    fAddObjectButton->UpdateEventDispatcher();
}

void EObjectView::RenderProperty(Engine::EProperty* storage, EString nameIdent) 
{
    EValueDescription propertyDsc = storage->GetDescription();
    EValueType type = propertyDsc.GetType();

    switch (type)
    {
    case EValueType::STRUCT: RenderStruct(static_cast<EStructProperty*>(storage), nameIdent); break;
    case EValueType::ARRAY: RenderArray(static_cast<EArrayProperty*>(storage), nameIdent); break;
    case EValueType::PRIMITIVE: RenderPrimitive(storage, nameIdent); break;
    case EValueType::ENUM: RenderEnum(static_cast<EEnumProperty*>(storage), nameIdent); break;
    case EValueType::UNKNOWN: break;
    }
}

void EObjectView::RenderStruct(EStructProperty* storage, EString nameIdent) 
{
    EValueDescription description = storage->GetDescription();
    if (ImGui::CollapsingHeader(storage->GetPropertyName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto& entry : description.GetStructFields())
        {
            const EString& propertyName = entry.first;
            RenderProperty(storage->GetProperty(propertyName), nameIdent + "." + propertyName);
        }
    }
}

void EObjectView::RenderPrimitive(Engine::EProperty* storage, EString nameIdent) 
{
    EValueDescription description = storage->GetDescription();
    const EString& primitiveId = description.GetId();
    if (primitiveId == E_TYPEID_STRING) { RenderString(static_cast<EValueProperty<EString>*>(storage), nameIdent); } 
    else if (primitiveId == E_TYPEID_INTEGER) { RenderInteger(static_cast<EValueProperty<i32>*>(storage), nameIdent); }
    else if (primitiveId == E_TYPEID_UNSIGNED_INTEGER) { RenderInteger(static_cast<EValueProperty<u32>*>(storage), nameIdent); }
    else if (primitiveId == E_TYPEID_UNSIGNED_BIG_INTEGER) { RenderInteger(static_cast<EValueProperty<u64>*>(storage), nameIdent); }
    else if (primitiveId == E_TYPEID_DOUBLE) { RenderDouble(static_cast<EValueProperty<double>*>(storage), nameIdent); }
    else if (primitiveId == E_TYPEID_BOOL) { RenderBool(static_cast<EValueProperty<bool>*>(storage), nameIdent); }
}

char* convert_str_to_chr(const std::string & s)
{
   char *pc = new char[s.size()+1];
   std::strcpy(pc, s.c_str());
   return pc; 
}

void EObjectView::RenderEnum(Engine::EEnumProperty* storage, EString nameIdent) 
{
    EValueDescription description = storage->GetDescription();
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

    storage->SetCurrentValue(description.GetEnumOptions()[currentItem]);
}

void EObjectView::RenderArray(Engine::EArrayProperty* storage, EString nameIdent) 
{
    EVector<EProperty*> elements = storage->GetElements();
    for (size_t i = 0; i < elements.size(); i++)
    {
        EProperty* element = elements[i];
        RenderProperty(element, nameIdent + "." + std::to_string(i));
    }

    if (ImGui::Button("Add Element"))
    {
        //storage->AddElement();
        storage->AddElement();
        EJson property = ESerializer::WritePropertyToJs(storage);
        shared::SetValue(fSelectedEntity, nameIdent, property.dump());
    }
}

void EObjectView::RenderBool(Engine::EValueProperty<bool>* storage, EString nameIdent) 
{
    bool value = storage->GetValue();
    ImGui::PushID(storage);
    if (ImGui::Checkbox(storage->GetPropertyName().c_str(), &value))
    {
        shared::SetValue<bool>(fSelectedEntity, nameIdent, value);
    }
    ImGui::PopID();
}

void EObjectView::RenderInteger(Engine::EValueProperty<i32>* storage, EString nameIdent) 
{
    i32 value = storage->GetValue();
    ImGui::PushID(storage);
    if (ImGui::InputInt(storage->GetPropertyName().c_str(), &value, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        shared::SetValue<i32>(fSelectedEntity, nameIdent, value);
    }
    ImGui::PopID();
}

void EObjectView::RenderInteger(Engine::EValueProperty<u32>* storage, EString nameIdent) 
{
    u32 value = storage->GetValue();
    ImGui::PushID(storage);
    if (ImGui::InputInt(storage->GetPropertyName().c_str(), (int*) &value, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        shared::SetValue<u32>(fSelectedEntity, nameIdent, value);
    }
    ImGui::PopID();
}

void EObjectView::RenderInteger(Engine::EValueProperty<u64>* storage, EString nameIdent) 
{
    int value = (int) storage->GetValue();
    ImGui::PushID(storage);
    if (ImGui::InputInt(storage->GetPropertyName().c_str(), &value, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        shared::SetValue<int>(fSelectedEntity, nameIdent, value);
    }
    ImGui::PopID();
}

void EObjectView::RenderDouble(Engine::EValueProperty<double>* storage, EString nameIdent) 
{
    double value = storage->GetValue();
    ImGui::PushID(storage);
    if (ImGui::InputDouble(storage->GetPropertyName().c_str(), &value, 0.0, 0.0, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue))
    {
        shared::SetValue<double>(fSelectedEntity, nameIdent, value);
    }
    ImGui::PopID();
}

void EObjectView::RenderString(Engine::EValueProperty<EString>* storage, EString nameIdent) 
{
    EString value = storage->GetValue();
    char buf[255];
    strcpy(buf, value.c_str());
    ImGui::PushID(storage);
    if (ImGui::InputText(storage->GetPropertyName().c_str(), buf, 255, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        shared::SetValue<EString>(fSelectedEntity, nameIdent, buf);
    }
    ImGui::PopID();
}

bool EObjectView::HasSelectedComponent(Engine::EValueDescription dsc) 
{
    return std::find_if(fSelectedComponents.begin(), fSelectedComponents.end(), [dsc](ERef<Engine::EProperty> property){
        return property->GetDescription() == dsc;
    }) != fSelectedComponents.end();
}
