#include "editor_extension.h"

#include "imgui_internal.h"

using namespace Engine;

EViewportTool::EViewportTool(const EString& toolName) 
    : fToolName(toolName)
{
    
}

bool EViewportTool::Render() 
{
    return OnRender();
}

bool EViewportTool::IsVisible() const
{
    return fVisible;
}

void EViewportTool::SetVisible(bool visible) 
{
    fVisible = visible;
}

const EString& EViewportTool::GetToolName() const
{
    return fToolName;
}

EString EViewportTool::GetIcon() const
{
    return ICON_MD_CALL_MADE;
}

void EViewportTool::SetViewport(EUIViewport* viewport) 
{
    fViewport = viewport;
}

EUIViewport* EViewportTool::GetViewport() const
{
    return fViewport;
}

void EViewportTool::ViewportClicked(const EVec2& screenPos, const EVec3& worldPos) 
{
    OnViewportClicked(screenPos, worldPos);
}

EPointMoveTool::EPointMoveTool() 
    : EViewportTool("POINT_MOVE"), fCenterPosition(100.0f, 100.0f)
{
    
}

bool EPointMoveTool::OnRender() 
{
    ImGuiContext* g = ImGui::GetCurrentContext();
    ImRect itemRect = g->LastItemData.Rect;

    EVec2 currentCenter = fCenterPosition + EVec2(itemRect.Min.x, itemRect.Min.y );
    float halfSize = 4.0f;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRect({currentCenter.x - halfSize, currentCenter.y - halfSize}, {currentCenter.x + halfSize, currentCenter.y + halfSize}, 0xffffffff);
    //draw_list->AddCircleFilled({fCenterPosition.x + itemRect.Min.x, fCenterPosition.y + itemRect.Min.y}, 10.0f, 0xffffffff);
    return false;
}

ELineEditTool::ELineEditTool()
    : EViewportTool(sGetName()), fLine(nullptr), fEditState(EditState::CREATING)
{
    SetVisible(false);
}

void ELineEditTool::SetLine(Renderer::RLine* line) 
{
    fLine = line;
    SetVisible(fLine != nullptr);   
}

Renderer::RLine* ELineEditTool::GetLine() const
{
    return fLine;
}

bool ELineEditTool::OnRender() 
{
    if (!fLine) { return false; }
    ImGuiIO& io = ImGui::GetIO();
    ImGuiContext* g = ImGui::GetCurrentContext();

    ImRect itemRect = g->LastItemData.Rect;

    float halfSize = 4.0f;

    EVec2 startPoint = GetViewport()->Project(fLine->GetStart()) + ImConvert::ImToGlmVec2(itemRect.Min);
    EVec2 endPoint = GetViewport()->Project(fLine->GetEnd()) + ImConvert::ImToGlmVec2(itemRect.Min);

    const ImRect startAnchor(ImConvert::GlmToImVec2(startPoint - EVec2(halfSize, halfSize)),ImConvert::GlmToImVec2(startPoint + EVec2(halfSize, halfSize)));
    const ImRect endAnchor(ImConvert::GlmToImVec2(endPoint - EVec2(halfSize, halfSize)),ImConvert::GlmToImVec2(endPoint + EVec2(halfSize, halfSize)));



    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRect(startAnchor.Min, startAnchor.Max, 0xffffffff);
    draw_list->AddRect(endAnchor.Min, endAnchor.Max, 0xffffffff);

    bool wasChanged = fCurrentSelection != Selection::NONE && !io.MouseDown[0];
    Selection newSelection = io.MouseDown[0] ? fCurrentSelection : Selection::NONE; 


    if (startAnchor.Contains(io.MousePos))
    {
        if (io.MouseDown[0])
        {
            newSelection = Selection::START;
        }
    }
    if (endAnchor.Contains(io.MousePos))
    {
        if (io.MouseDown[0])
        {
            newSelection = Selection::END;
        }
    }

    fCurrentSelection = newSelection;

    switch (fCurrentSelection)
    {
    case Selection::START:
        fLine->SetStart(GetViewport()->Unproject(EVec3(io.MousePos.x - itemRect.Min.x, io.MousePos.y - itemRect.Min.y, 0.0f)));
        break;
    case Selection::END:
        fLine->SetEnd(GetViewport()->Unproject(EVec3(io.MousePos.x - itemRect.Min.x, io.MousePos.y - itemRect.Min.y, 0.0f)));
        break;
    case Selection::NONE:
        break;
    }

    return wasChanged;
}

void ELineEditTool::OnViewportClicked(const EVec2& screenPos, const EVec3& worldPos) 
{
    
}

void ELineEditTool::OnActivateTool()
{
    fEditState = EditState::CREATING;
}

EString ELineEditTool::sGetName()
{
    return "LINE_EDIT";
}

EString ELineEditTool::GetIcon() const
{
    return ICON_MD_CALL_MADE;
}



EBezierEditTool::EBezierEditTool() 
    : EViewportTool(sGetName()), fCurve(nullptr)
{
    fCurrentSelection = Selection::NONE;
    SetVisible(false);
}

bool EBezierEditTool::OnRender() 
{
    if (!fCurve) { return false; }
    ImGuiIO& io = ImGui::GetIO();
    ImGuiContext* g = ImGui::GetCurrentContext();

    ImRect itemRect = g->LastItemData.Rect;

    float halfSize = 4.0f;

    EVec2 startPoint = GetViewport()->Project(fCurve->GetStartPos()) + ImConvert::ImToGlmVec2(itemRect.Min);
    EVec2 endPoint = GetViewport()->Project(fCurve->GetEndPos()) + ImConvert::ImToGlmVec2(itemRect.Min);
    EVec2 controllPoint1 = GetViewport()->Project(fCurve->GetControll1()) + ImConvert::ImToGlmVec2(itemRect.Min);
    EVec2 controllPoint2 = GetViewport()->Project(fCurve->GetControll2()) + ImConvert::ImToGlmVec2(itemRect.Min);

    const ImRect startAnchor(ImConvert::GlmToImVec2(startPoint - EVec2(halfSize, halfSize)),ImConvert::GlmToImVec2(startPoint + EVec2(halfSize, halfSize)));
    const ImRect endAnchor(ImConvert::GlmToImVec2(endPoint - EVec2(halfSize, halfSize)),ImConvert::GlmToImVec2(endPoint + EVec2(halfSize, halfSize)));
    const ImRect ctrl1Anchor(ImConvert::GlmToImVec2(controllPoint1 - EVec2(halfSize, halfSize)),ImConvert::GlmToImVec2(controllPoint1 + EVec2(halfSize, halfSize)));
    const ImRect ctrl2Anchor(ImConvert::GlmToImVec2(controllPoint2 - EVec2(halfSize, halfSize)),ImConvert::GlmToImVec2(controllPoint2 + EVec2(halfSize, halfSize)));



    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRect(startAnchor.Min, startAnchor.Max, 0xffffffff);
    draw_list->AddRect(endAnchor.Min, endAnchor.Max, 0xffffffff);
    draw_list->AddRect(ctrl1Anchor.Min, ctrl1Anchor.Max, 0xffffffff);
    draw_list->AddRect(ctrl2Anchor.Min, ctrl2Anchor.Max, 0xffffffff);

    bool wasChanged = fCurrentSelection != Selection::NONE && !io.MouseDown[0];
    Selection newSelection = io.MouseDown[0] ? fCurrentSelection : Selection::NONE;


    if (startAnchor.Contains(io.MousePos))
    {
        if (io.MouseDown[0])
        {
            newSelection = Selection::START;
        }
    }
    if (endAnchor.Contains(io.MousePos))
    {
        if (io.MouseDown[0])
        {
            newSelection = Selection::END;
        }
    }
    if (ctrl1Anchor.Contains(io.MousePos))
    {
        if (io.MouseDown[0])
        {
            newSelection = Selection::CTRL1;
        }
    }
    if (ctrl2Anchor.Contains(io.MousePos))
    {
        if (io.MouseDown[0])
        {
            newSelection = Selection::CTRL2;
        }
    }

    fCurrentSelection = newSelection;


    switch (fCurrentSelection)
    {
    case Selection::START:
        fCurve->SetStartPos(GetViewport()->Unproject(EVec3(io.MousePos.x - itemRect.Min.x, io.MousePos.y - itemRect.Min.y, 0.0f)));
        break;
    case Selection::END:
        fCurve->SetEndPos(GetViewport()->Unproject(EVec3(io.MousePos.x - itemRect.Min.x, io.MousePos.y - itemRect.Min.y, 0.0f)));
        break;
    case Selection::CTRL1:
        fCurve->SetControll1(GetViewport()->Unproject(EVec3(io.MousePos.x - itemRect.Min.x, io.MousePos.y - itemRect.Min.y, 0.0f)));
        break;
    case Selection::CTRL2:
        fCurve->SetControll2(GetViewport()->Unproject(EVec3(io.MousePos.x - itemRect.Min.x, io.MousePos.y - itemRect.Min.y, 0.0f)));
        break;
    case Selection::NONE:
        break;
    }


    draw_list->AddLine(ImConvert::GlmToImVec2(startPoint), ImConvert::GlmToImVec2(controllPoint1), 0xffffffff);
    draw_list->AddLine(ImConvert::GlmToImVec2(endPoint), ImConvert::GlmToImVec2(controllPoint2), 0xffffffff);
    return wasChanged;
}

void EBezierEditTool::SetBezierCurve(Renderer::RBezierCurve* curve) 
{
    fCurve = curve;
    SetVisible(fCurve != nullptr);
}


Renderer::RBezierCurve* EBezierEditTool::GetCurve() const
{
    return fCurve;
}

EString EBezierEditTool::sGetName()
{
    return "BEZIER_EDIT";
}

EString EBezierEditTool::GetIcon() const
{
    return ICON_MD_ROUNDED_CORNER;
}