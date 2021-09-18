#include "editor.h"

using namespace Editor;
using namespace Engine;

EExtensionView::EExtensionView() 
 : EUIField("ExtensionView")
{
    
}

EExtensionView::~EExtensionView() 
{
    
}

bool EExtensionView::OnRender() 
{
    {
        EVector<EExtension*> loadedExtensions = EExtensionManager::instance().GetLoadedExtensions();
        for (EExtension* ext : loadedExtensions)
        {
            EString name = ext->GetName();
            EString fullPath = ext->GetFilePath();
            ImGui::Text("%s", name.c_str());
            ImGui::SameLine();
            if (ImGui::Button("Reload"))
            {
                EExtensionManager::instance().ReloadExtension(ext);
                // The ext ptr is invald after here. 
                break; // For now we just break. Cases other extensions to disappear for one frame 
            }
            ImGui::SameLine();
            bool isLoaded = EExtensionManager::instance().IsLoaded(name);
            if (ImGui::Checkbox("Loaded", &isLoaded))
            {
                if (isLoaded)
                {
                    EExtensionManager::instance().LoadExtension(fullPath);
                }
                else
                {
                    EExtensionManager::instance().UnloadExtension(ext);
                }
            }
        }
        // Refetch the loaded extensions. If the reload was clicked one of the pointers is invalid. Refetching them solves the problem!
    }
    if (ImGui::Button("Load Extension"))
    {
        EVector<EString> loadingPaths = Graphics::Wrapper::OpenFileDialog("Load Extension", {"uex"});
        for (const EString& extPath : loadingPaths)
        {
            EExtensionManager::instance().LoadExtension(extPath);
        }
    }
    return true;
}

EResourceView::EResourceView() 
    : EUIField("RESOURCE_MANAGER"), selectedResource(0)
{
    
}

bool EResourceView::OnRender() 
{
    ImGui::BeginChild("Resources", {150, 0});
    for (EResourceData* data : EExtensionManager::instance().GetActiveScene()->GetResourceManager().GetAllResource())
    {
        if (ImGui::Selectable(data->Name.c_str()))
        {
            selectedResource = selectedResource == data->ID ? 0 : data->ID;
        }
    }
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("ResourceDetail");
    if (selectedResource)
    {
        ImGui::LabelText("ID: ", "%llu", selectedResource);
    }
    ImGui::EndChild();
    return true;
}

EObjectView::EObjectView()
    : EUIField("OBJECTVIEW"), fSelectedEntity(0)
{
    fAddObjectButton = EMakeRef<EUIButton>("Add Object");
    fAddObjectButton->AddEventListener<events::EButtonEvent>([this](){
        EExtensionManager::instance().GetActiveScene()->CreateEntity();
    });
}

bool EObjectView::OnRender() 
{
    if (!EExtensionManager::instance().GetActiveScene()) { return false; }

    ImGui::BeginChild("Entity Child", {100, 0});
    for (ERegister::Entity entity : EExtensionManager::instance().GetActiveScene()->GetAllEntities())
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
        for (EStructProperty* storage : EExtensionManager::instance().GetActiveScene()->GetAllComponents(fSelectedEntity))
        {
            RenderProperty(storage);
        }

        if (ImGui::Button("Add Component"))
        {
            ImGui::OpenPopup("add-component-popup");
        }
        if (ImGui::BeginPopup("add-component-popup"))
        {
            for (EValueDescription compDsc : EExtensionManager::instance().GetTypeRegister().GetAllItems())
            {
                if (compDsc.GetType() != EValueType::STRUCT) { continue; }
                bool hasComp = EExtensionManager::instance().GetActiveScene()->HasComponent(fSelectedEntity, compDsc);
                if (!hasComp)
                {
                    if (ImGui::Selectable(compDsc.GetId().c_str()))
                    {
                        EExtensionManager::instance().GetActiveScene()->AddComponent(fSelectedEntity, compDsc);
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

void EObjectView::RenderProperty(Engine::EProperty* storage) 
{
    EValueDescription propertyDsc = storage->GetDescription();
    EValueType type = propertyDsc.GetType();

    switch (type)
    {
    case EValueType::STRUCT: RenderStruct(static_cast<EStructProperty*>(storage)); break;
    case EValueType::ARRAY: RenderArray(static_cast<EArrayProperty*>(storage)); break;
    case EValueType::PRIMITIVE: RenderPrimitive(storage); break;
    case EValueType::ENUM: RenderEnum(static_cast<EEnumProperty*>(storage)); break;
    case EValueType::UNKNOWN: break;
    }
}

void EObjectView::RenderStruct(EStructProperty* storage) 
{
    EValueDescription description = storage->GetDescription();
    if (ImGui::CollapsingHeader(storage->GetPropertyName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto& entry : description.GetStructFields())
        {
            const EString& propertyName = entry.first;
            RenderProperty(storage->GetProperty(propertyName));
        }
    }
}

void EObjectView::RenderPrimitive(Engine::EProperty* storage) 
{
    EValueDescription description = storage->GetDescription();
    const EString& primitiveId = description.GetId();
    if (primitiveId == E_TYPEID_STRING) { RenderString(static_cast<EValueProperty<EString>*>(storage)); } 
    else if (primitiveId == E_TYPEID_INTEGER) { RenderInteger(static_cast<EValueProperty<i32>*>(storage)); }
    else if (primitiveId == E_TYPEID_UNSIGNED_INTEGER) { RenderInteger(static_cast<EValueProperty<u32>*>(storage)); }
    else if (primitiveId == E_TYPEID_UNSIGNED_BIG_INTEGER) { RenderInteger(static_cast<EValueProperty<u64>*>(storage)); }
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

void EObjectView::RenderArray(Engine::EArrayProperty* storage) 
{
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

void EObjectView::RenderInteger(Engine::EValueProperty<u32>* storage) 
{
    u32 value = storage->GetValue();
    ImGui::PushID(storage);
    ImGui::InputInt(storage->GetPropertyName().c_str(), (int*) &value);
    ImGui::PopID();
    storage->SetValue(value);
}

void EObjectView::RenderInteger(Engine::EValueProperty<u64>* storage) 
{
    int value = (int) storage->GetValue();
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

ECommandLine::ECommandLine(EChaiContext* context) 
    : EUIField("CommandLine"), fChaiContext(context)
{
    
}

bool ECommandLine::OnRender() 
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - 32));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, 32));
	ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = 0
		| ImGuiWindowFlags_NoDocking 
		| ImGuiWindowFlags_NoTitleBar 
		| ImGuiWindowFlags_NoResize 
		| ImGuiWindowFlags_NoMove 
		| ImGuiWindowFlags_NoScrollbar 
		| ImGuiWindowFlags_NoSavedSettings
		;


    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::Begin("CommandLine", NULL, window_flags);
    ImGui::PopStyleVar();
    ImGui::PushItemWidth(-1);
    char buffer[255];
    memset(buffer, 0, 255);
    if (ImGui::InputText("##COMMAND_LINE_INPUT", buffer, 255, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        fChaiContext->Execute(buffer);
    }
    ImGui::PopItemWidth();
    ImGui::End();
    return true;
}