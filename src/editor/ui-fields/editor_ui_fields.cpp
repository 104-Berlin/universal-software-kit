#include "editor.h"

using namespace Editor;
using namespace Engine;


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
        EString command = EStringUtil::ToLower(buffer);
        
        //fChaiContext->Execute(buffer);
        if (command == "add")
        {
            E_INFO("NEW ENTITY");
            shared::CreateEntity();
        }
    }
    ImGui::PopItemWidth();
    ImGui::End();
    return true;
}

void Spinner(const char* label, float radius, int thickness, const ImU32& color) {

    ImGuiWindow* window = ImGui::GetCurrentWindow();

    ImGuiContext& g = *ImGui::GetCurrentContext();
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size((radius )*2, (radius + style.FramePadding.y)*2);
    
    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return;
    
    // Render
    window->DrawList->PathClear();
    
    int num_segments = 30;
    int start = abs(ImSin(g.Time*1.8f)*(num_segments-5));
    
    const float a_min = IM_PI*2.0f * ((float)start) / (float)num_segments;
    const float a_max = IM_PI*2.0f * ((float)num_segments-3) / (float)num_segments;

    const ImVec2 centre = ImVec2(pos.x+radius, pos.y+radius+style.FramePadding.y);
    
    for (int i = 0; i < num_segments; i++) {
        const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
        window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a+g.Time*8) * radius,
                                            centre.y + ImSin(a+g.Time*8) * radius));
    }

    window->DrawList->PathStroke(color, false, thickness);
}

EServerStatusUI::EServerStatusUI()
    : EUIField("ServerStatus"), fCurrentPort(1420)
{
    
}

bool EServerStatusUI::OnRender() 
{
    switch (shared::StaticSharedContext::instance().GetRegisterConnection().GetConnectionStatus())
    {
    case ERegisterConnection::Status::Connected:
        ImGui::PushStyleColor(ImGuiCol_Text, 0xff00fe21);
        ImGui::Text("Connected to %s", shared::StaticSharedContext::instance().GetRegisterConnection().GetConnectedToAddress().c_str());
        ImGui::PopStyleColor();
        break;
    case ERegisterConnection::Status::Connecting:
        ImGui::Text("Connecting");
        ImGui::SameLine();
        Spinner("Connecting", ImGui::GetTextLineHeight() / 2.0f, 4.0f, 0xffffffff);
        break;
    case ERegisterConnection::Status::Disconnected:
        ImGui::PushStyleColor(ImGuiCol_Text, 0xff0021fe);
        ImGui::Text("Not Connected");
        ImGui::PopStyleColor();
        break;
    }

    if (shared::StaticSharedContext::instance().IsLocaleServerRunning())
    {
        ImGui::Text("Locale Server is running");
    }
    else
    {
        ImGui::InputInt("Port", &fCurrentPort);
        fCurrentPort = std::clamp(fCurrentPort, 1024, 65535); // Port min max
        ImGui::SameLine();
        if (ImGui::Button("Start locale server"))
        {
            shared::StaticSharedContext::instance().RestartLocaleServer(fCurrentPort);
        }
    }

    return true;
}
