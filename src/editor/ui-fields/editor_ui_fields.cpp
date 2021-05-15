#include "editor.h"

using namespace Editor;
using namespace Engine;

EExtensionView::EExtensionView(Engine::EExtensionManager* extensionManager) 
 : EUIField("ExtensionView"), fExtensionManager(extensionManager)
{
    
}

EExtensionView::~EExtensionView() 
{
    
}

bool EExtensionView::OnRender() 
{
    EVector<EExtension*> loadedExtensions = fExtensionManager->GetLoadedExtensions();
    for (EExtension* ext : loadedExtensions)
    {
        const EString& name = ext->GetName();
        ImGui::Text("%s", name.c_str());
        ImGui::SameLine();
        if (ImGui::Button("Reload"))
        {
            // Load the extension
            E_INFO("reload not implemented for extensions!");
        }
    }
    if (ImGui::Button("Load Extension"))
    {
        EVector<EString> loadingPaths = Graphics::Wrapper::OpenFileDialog("Load Extension", {"uex"});
        for (const EString& extPath : loadingPaths)
        {
            fExtensionManager->LoadExtension(extPath);
        }
    }
    return true;
}

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
        for (EStructProperty* storage : fScene->GetAllComponents(fSelectedEntity))
        {
            RenderStruct(storage);
        }

        if (ImGui::Button("Add Component"))
        {
            ImGui::OpenPopup("add-component-popup");
        }
        if (ImGui::BeginPopup("add-component-popup"))
        {
            for (EValueDescription* compDsc : ETypeRegister::get().GetAllDescriptions())
            {
                if (compDsc->GetType() != EValueType::STRUCT) { continue; }
                bool hasComp = fScene->HasComponent(fSelectedEntity, compDsc->GetId());
                if (!hasComp)
                {
                    if (ImGui::Selectable(compDsc->GetId().c_str()))
                    {
                        fScene->InsertComponent(fSelectedEntity, compDsc->GetId());
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

void EObjectView::RenderStruct(EStructProperty* storage) 
{
    EStructDescription* description = static_cast<EStructDescription*>(storage->GetDescription());
    if (ImGui::CollapsingHeader(storage->GetPropertyName().c_str()))
    {
        for (auto& entry : description->GetFields())
        {
            const EString& propertyName = entry.first;
            EValueDescription* fieldDsc = entry.second;
            EValueType fieldType = fieldDsc->GetType();
            switch (fieldType)
            {
            case EValueType::STRUCT: RenderStruct(static_cast<EStructProperty*>(storage->GetProperty(propertyName))); break;
            case EValueType::PRIMITIVE: RenderPrimitive(storage->GetProperty(propertyName)); break;
            }    
        }
    }
}

void EObjectView::RenderPrimitive(Engine::EProperty* storage) 
{
    EValueDescription* description = storage->GetDescription();
    const EString& primitiveId = description->GetId();
    if (primitiveId == E_TYPEID_STRING) { RenderString(static_cast<EValueProperty<EString>*>(storage)); } 
    else if (primitiveId == E_TYPEID_INTEGER) { RenderInteger(static_cast<EValueProperty<i32>*>(storage)); }
    else if (primitiveId == E_TYPEID_DOUBLE) { RenderDouble(static_cast<EValueProperty<double>*>(storage)); }
    else if (primitiveId == E_TYPEID_BOOL) { RenderBool(static_cast<EValueProperty<bool>*>(storage)); }
}

void EObjectView::RenderBool(Engine::EValueProperty<bool>* storage) 
{
    bool value = storage->GetValue();
    ImGui::Checkbox(storage->GetPropertyName().c_str(), &value);
    storage->SetValue(value);
}

void EObjectView::RenderInteger(Engine::EValueProperty<i32>* storage) 
{
    i32 value = storage->GetValue();
    ImGui::InputInt(storage->GetPropertyName().c_str(), &value);
    storage->SetValue(value);
}

void EObjectView::RenderDouble(Engine::EValueProperty<double>* storage) 
{
    double value = storage->GetValue();
    ImGui::InputDouble(storage->GetPropertyName().c_str(), &value);
    storage->SetValue(value);
}

void EObjectView::RenderString(Engine::EValueProperty<EString>* storage) 
{
    EString value = storage->GetValue();
    char buf[255];
    strcpy(buf, value.c_str());
    ImGui::InputText(storage->GetPropertyName().c_str(), buf, 255);
    storage->SetValue(buf);
}