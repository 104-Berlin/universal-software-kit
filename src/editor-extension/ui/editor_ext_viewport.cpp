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

EBezierEditTool::EBezierEditTool() 
    : EViewportTool("BEZIER_EDIT"), fCurve(nullptr)
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

    EVec2 startPoint = EVec2(fCurve->GetStartPos().x, fCurve->GetStartPos().y) + ImConvert::ImToGlmVec2(itemRect.Min);
    EVec2 endPoint = EVec2(fCurve->GetEndPos().x, fCurve->GetEndPos().y) + ImConvert::ImToGlmVec2(itemRect.Min);
    EVec2 controllPoint1 = EVec2(fCurve->GetControll1().x, fCurve->GetControll1().y) + ImConvert::ImToGlmVec2(itemRect.Min);
    EVec2 controllPoint2 = EVec2(fCurve->GetControll2().x, fCurve->GetControll2().y) + ImConvert::ImToGlmVec2(itemRect.Min);

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
        fCurve->SetStartPos(EVec3(io.MousePos.x - itemRect.Min.x, io.MousePos.y - itemRect.Min.y, 0.0f));
        break;
    case Selection::END:
        fCurve->SetEndPos(EVec3(io.MousePos.x - itemRect.Min.x, io.MousePos.y - itemRect.Min.y, 0.0f));
        break;
    case Selection::CTRL1:
        fCurve->SetControll1(EVec3(io.MousePos.x - itemRect.Min.x, io.MousePos.y - itemRect.Min.y, 0.0f));
        break;
    case Selection::CTRL2:
        fCurve->SetControll2(EVec3(io.MousePos.x - itemRect.Min.x, io.MousePos.y - itemRect.Min.y, 0.0f));
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


EUIViewport::EUIViewport(const Renderer::RCamera& camera) 
    :   EUIField("VIEWPORT"), 
        fFrameBuffer(Graphics::Wrapper::CreateFrameBuffer(100, 100)), 
        fRenderer(Graphics::Wrapper::GetMainContext(), fFrameBuffer),
        fCamera(camera)
{
    
}

EUIViewport::~EUIViewport() 
{
    for (EViewportTool* tool : fRegisteredTools)
    {
        delete tool;
    }
    fRegisteredTools.clear();

    if (fFrameBuffer)
    {
        delete fFrameBuffer;
    }
}

Renderer::RScene& EUIViewport::GetScene() 
{
    return fScene;
}

const Renderer::RScene& EUIViewport::GetScene() const
{
    return fScene;
}

const Renderer::RCamera& EUIViewport::GetCamera() const
{
    return fCamera;
}

Renderer::RCamera& EUIViewport::GetCamera() 
{
    return fCamera;
}

EViewportTool* EUIViewport::AddTool(EViewportTool* newTool) 
{
    fRegisteredTools.push_back(newTool);
    return newTool;
}

bool EUIViewport::OnRender() 
{
    ImVec2 contentRegion = ImGui::GetContentRegionAvail();

    fFrameBuffer->Resize(contentRegion.x, contentRegion.y, Graphics::GFrameBufferFormat::RGBA8);
    fRenderer.Render(&fScene, &fCamera);
    ImGui::Image((ImTextureID)(unsigned long long)(unsigned long)fFrameBuffer->GetColorAttachment(), contentRegion, {0, 1}, {1, 0});

    for (EViewportTool* tool : fRegisteredTools)
    {
        if (tool->IsVisible())
        {
            if (tool->Render())
            {
                fEventDispatcher.Enqueue<events::EViewportToolFinishEvent>({tool->GetToolName()});
            }
        }
    }

    return true;
}