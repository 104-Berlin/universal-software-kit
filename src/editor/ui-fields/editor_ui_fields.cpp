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

EResourceView::EResourceView(Engine::EResourceManager* resourceManager) 
    : EUIField("RESOURCE_MANAGER"), fResourceManager(resourceManager)
{
    
}

bool EResourceView::OnRender() 
{
    
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
            RenderProperty(storage);
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

void EObjectView::RenderProperty(Engine::EProperty* storage) 
{
    EValueDescription* propertyDsc = storage->GetDescription();
    EValueType type = propertyDsc->GetType();
    switch (type)
    {
    case EValueType::STRUCT: RenderStruct(static_cast<EStructProperty*>(storage)); break;
    case EValueType::PRIMITIVE: RenderPrimitive(storage); break;
    case EValueType::ENUM: RenderEnum(static_cast<EEnumProperty*>(storage)); break;
    case EValueType::ARRAY: RenderArray(static_cast<EArrayProperty*>(storage)); break;
    }
}

void EObjectView::RenderStruct(EStructProperty* storage) 
{
    EStructDescription* description = static_cast<EStructDescription*>(storage->GetDescription());
    if (ImGui::CollapsingHeader(storage->GetPropertyName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto& entry : description->GetFields())
        {
            const EString& propertyName = entry.first;
            RenderProperty(storage->GetProperty(propertyName));
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

char* convert_str_to_chr(const std::string & s)
{
   char *pc = new char[s.size()+1];
   std::strcpy(pc, s.c_str());
   return pc; 
}

void EObjectView::RenderEnum(Engine::EEnumProperty* storage) 
{
    EEnumDescription* description = static_cast<EEnumDescription*>(storage->GetDescription());
    int currentItem = -1;
    for (EString option : description->GetOptions())
    {
        currentItem++;
        if (option == storage->GetCurrentValue())
        {
            break;
        }
    }
    std::vector<char*> opt;
    std::transform(description->GetOptions().begin(), description->GetOptions().end(), std::back_inserter(opt), convert_str_to_chr);

    ImGui::Combo(storage->GetPropertyName().c_str(), &currentItem, opt.data(), description->GetOptions().size());

    for ( size_t i = 0 ; i < opt.size() ; i++ )
            delete [] opt[i];

    storage->SetCurrentValue(description->GetOptions()[currentItem]);
}

void EObjectView::RenderArray(Engine::EArrayProperty* storage) 
{
    EArrayDescription* arrayDsc = static_cast<EArrayDescription*>(storage->GetDescription());

    for (EProperty* element : storage->GetElements())
    {
        RenderProperty(element);
    }

    if (ImGui::Button("Add Element"))
    {
        storage->AddElement();
    }
}

void EObjectView::RenderBool(Engine::EValueProperty<bool>* storage) 
{
    bool value = storage->GetValue();
    ImGui::PushID(storage);
    ImGui::Checkbox(storage->GetPropertyName().c_str(), &value);
    ImGui::PopID();
    storage->SetValue(value);
}

void EObjectView::RenderInteger(Engine::EValueProperty<i32>* storage) 
{
    i32 value = storage->GetValue();
    ImGui::PushID(storage);
    ImGui::InputInt(storage->GetPropertyName().c_str(), &value);
    ImGui::PopID();
    storage->SetValue(value);
}

void EObjectView::RenderDouble(Engine::EValueProperty<double>* storage) 
{
    double value = storage->GetValue();
    ImGui::PushID(storage);
    ImGui::InputDouble(storage->GetPropertyName().c_str(), &value);
    ImGui::PopID();
    storage->SetValue(value);
}

void EObjectView::RenderString(Engine::EValueProperty<EString>* storage) 
{
    EString value = storage->GetValue();
    char buf[255];
    strcpy(buf, value.c_str());
    ImGui::PushID(storage);
    ImGui::InputText(storage->GetPropertyName().c_str(), buf, 255);
    ImGui::PopID();
    storage->SetValue(buf);
}