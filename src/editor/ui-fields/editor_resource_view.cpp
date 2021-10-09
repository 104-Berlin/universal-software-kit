#include "editor.h"

using namespace Editor;
using namespace Engine;

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
