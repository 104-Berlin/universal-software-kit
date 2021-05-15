#include "editor.h"

using namespace Editor;
using namespace Engine;

EObjectView::EObjectView(Engine::EScene* scene)
    : EUIField("OBJECTVIEW"), fScene(scene), fSelectedEntity(0)
{
    fAddObjectButton = EMakeRef<EUIButton>("Add Object");
    fAddObjectButton->AddEventListener<Engine::EClickEvent>([this](){
        fScene->CreateEntity();
    });
}

bool EObjectView::OnRender() 
{
    if (!fScene) { return false; }

    ImGui::BeginChild("Entity Child", {100, 0});
    for (EScene::Entity entity : fScene->GetAllEntities())
    {
        EString entityIdent = "Entity " + std::to_string(entity);
        bool selected = fSelectedEntity == entity;
        if (ImGui::Selectable(entityIdent.c_str(), &selected))
        {
            fSelectedEntity = fSelectedEntity == entity ? 0 : entity;
        }
    }
    fAddObjectButton->Render();
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("ComponentChild");
    if (fSelectedEntity)
    {
        for (Engine::EComponentStorage storage : fScene->GetAllComponents(fSelectedEntity))
        {
            RenderComponentStorage(storage);
        }

        if (ImGui::Button("Add Component"))
        {
            ImGui::OpenPopup("add-component-popup");
        }
        if (ImGui::BeginPopup("add-component-popup"))
        {
            for (EComponentDescription compDsc : fScene->GetRegisteredComponents())
            {
                bool hasComp = fScene->HasComponent(fSelectedEntity, compDsc.ID);
                if (!hasComp)
                {
                    if (ImGui::Selectable(compDsc.ID.c_str()))
                    {
                        fScene->InsertComponent(fSelectedEntity, compDsc.ID);
                    }
                }
            }
            ImGui::EndPopup();
        }
    }
    ImGui::EndChild();

    //ImGui::ShowDemoWindow();


    return true;
}

void EObjectView::OnUpdateEventDispatcher()
{
    fAddObjectButton->UpdateEventDispatcher();
}

void EObjectView::RenderComponentStorage(Engine::EComponentStorage storage) 
{
    EComponentDescription description = storage.GetComponentDescription();
    for (auto& valueType : description.ValueTypeDesciptions)
    {
        switch (valueType.Type)
        {
        case EValueType::BOOL: RenderBool(valueType.Name, storage); break;
        case EValueType::DOUBLE: RenderDouble(valueType.Name, storage); break;
        case EValueType::INTEGER: RenderInteger(valueType.Name, storage); break;
        case EValueType::STRING: RenderString(valueType.Name, storage); break;
        case EValueType::COMPONENT_REFERENCE: break;
        }
    }
}

void EObjectView::RenderBool(const EString& name, Engine::EComponentStorage storage) 
{
    EValueProperty<bool>* boolProperty = nullptr;
    if (storage.GetProperty(name, &boolProperty))
    {
        bool value = boolProperty->GetValue();
        ImGui::Checkbox(name.c_str(), &value);
        boolProperty->SetValue(value);        
    }
}

void EObjectView::RenderDouble(const EString& name, Engine::EComponentStorage storage) 
{
    EValueProperty<double>* doubleProperty = nullptr;
    if (storage.GetProperty(name, &doubleProperty))
    {
        double value = doubleProperty->GetValue();
        ImGui::InputDouble(name.c_str(), &value);
        doubleProperty->SetValue(value);
    }
}

void EObjectView::RenderInteger(const EString& name, Engine::EComponentStorage storage) 
{
    EValueProperty<i32>* integerProperty = nullptr;
    if (storage.GetProperty(name, &integerProperty))
    {
        i32 value = integerProperty->GetValue();
        ImGui::InputInt(name.c_str(), &value);
        integerProperty->SetValue(value);
    }
}

void EObjectView::RenderString(const EString& name, Engine::EComponentStorage storage) 
{
    EValueProperty<EString>* stringProperty = nullptr;
    if (storage.GetProperty(name, &stringProperty))
    {
        EString value = stringProperty->GetValue();
        char buf[255];
        strcpy(buf, value.c_str());
        ImGui::InputText(name.c_str(), buf, 255);
        stringProperty->SetValue(buf);
    }
}