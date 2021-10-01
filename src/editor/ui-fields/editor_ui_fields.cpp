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
        EVector<EExtension*> loadedExtensions = shared::StaticSharedContext::instance().GetExtensionManager().GetLoadedExtensions();
        for (EExtension* ext : loadedExtensions)
        {
            EString name = ext->GetName();
            EString fullPath = ext->GetFilePath();
            ImGui::Text("%s", name.c_str());
            ImGui::SameLine();
            if (ImGui::Button("Reload"))
            {
                shared::StaticSharedContext::instance().GetExtensionManager().ReloadExtension(ext);
                // The ext ptr is invald after here. 
                break; // For now we just break. Cases other extensions to disappear for one frame 
            }
            ImGui::SameLine();
            bool isLoaded = shared::StaticSharedContext::instance().GetExtensionManager().IsLoaded(name);
            if (ImGui::Checkbox("Loaded", &isLoaded))
            {
                if (isLoaded)
                {
                    shared::StaticSharedContext::instance().GetExtensionManager().LoadExtension(fullPath);
                }
                else
                {
                    shared::StaticSharedContext::instance().GetExtensionManager().UnloadExtension(ext);
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
            shared::StaticSharedContext::instance().GetExtensionManager().LoadExtension(extPath);
        }
    }
    return true;
}

EResourceView::EResourceView() 
    : EUIField("RESOURCE_MANAGER"), selectedResource(0)
{
    shared::StaticSharedContext::instance().Events().GetEventDispatcher().Connect<events::EResourceAddedEvent>([this](events::EResourceAddedEvent event){
        ERef<EResourceData> data = shared::GetResource(event.ResourceID);
        if (data)
        {
            fResources.push_back({data->ID, data->Name, data->PathToFile});
        }
    });
}

bool EResourceView::OnRender() 
{
    ImGui::BeginChild("Resources", {150, 0});
    for (Resource& data : fResources)
    {
        if (ImGui::Selectable(data.Name.c_str()))
        {
            selectedResource = selectedResource == data.ID ? 0 : data.ID;
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

ECommandLine::ECommandLine() 
    : EUIField("CommandLine")
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
        //fChaiContext->Execute(buffer);
    }
    ImGui::PopItemWidth();
    ImGui::End();
    return true;
}
