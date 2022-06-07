#include "editor_extension.h"

#include "imgui_internal.h"

using namespace Engine;

EViewportTool::EViewportTool(const EString& toolName, const EString& componentIdentifier) 
    : fVisible(true), fToolName(toolName), fComponentIdentifier(componentIdentifier), fViewport(nullptr)
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

void EViewportTool::SetComponentIdentifier(const EString& ident)
{
    fComponentIdentifier = ident;
}

const EString& EViewportTool::GetComponentIdentifer() const
{
    return fComponentIdentifier;
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

void EViewportTool::ActivateTool()
{
    OnActivateTool();
}

void EViewportTool::Finish()
{
    if (!GetActiveObject()) { return; }
    OnFinished(fViewport->GetEntityFromObject(GetActiveObject()));
}

Renderer::RObject* EViewportTool::GetActiveObject() const
{
    if (!fViewport) { return nullptr; }
    return fViewport->GetSelectionContext().GetSelectedObject();
}



ELineEditTool::ELineEditTool()
    : EViewportTool(sGetName()), fEditState(EditState::CREATING)
{
}

bool ELineEditTool::OnRender() 
{
    Renderer::RLine* line = dynamic_cast<Renderer::RLine*>(GetActiveObject());

    if (!line) { return false; }
    ImGuiIO& io = ImGui::GetIO();
    ImGuiContext* g = ImGui::GetCurrentContext();

    ImRect itemRect = g->LastItemData.Rect;

    float halfSize = 4.0f;

    EVec2 startPoint = GetViewport()->Project(line->GetStart()) + ImConvert::ImToGlmVec2(itemRect.Min);
    EVec2 endPoint = GetViewport()->Project(line->GetEnd()) + ImConvert::ImToGlmVec2(itemRect.Min);

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
        line->SetStart(GetViewport()->Unproject(EVec3(io.MousePos.x - itemRect.Min.x, io.MousePos.y - itemRect.Min.y, 0.0f)));
        break;
    case Selection::END:
        line->SetEnd(GetViewport()->Unproject(EVec3(io.MousePos.x - itemRect.Min.x, io.MousePos.y - itemRect.Min.y, 0.0f)));
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

void ELineEditTool::OnFinished(EDataBase::Entity entity)
{
    if (entity)
    {
        shared::SetValue(entity, GetComponentIdentifer(), fCurrentLine);
    }
}


EBezierEditTool::EBezierEditTool() 
    : EViewportTool(sGetName()), fLastSelection(Selection::NONE)
{
}

bool EBezierEditTool::OnRender() 
{
    if (!GetActiveObject())
    {
        return false;
    }
    Renderer::RBezierCurve* curve = GetActiveObject()->FindTypeOf<Renderer::RBezierCurve>(true);

    if (!curve) { return false; }
    
    EMat4 startMatrix = curve->GetStartPositionObject()->GetModelMatrix();
    EMat4 endMatrix = curve->GetEndPositionObject()->GetModelMatrix();
    EMat4 controll1Matrix = curve->GetControllPoint1Object()->GetModelMatrix();
    EMat4 controll2Matrix = curve->GetControllPoint2Object()->GetModelMatrix();


    Selection changedPoint = HandleManipulate(startMatrix, endMatrix, controll1Matrix, controll2Matrix);

    switch (changedPoint)
    {
    case Selection::START:
    {
        curve->GetStartPositionObject()->SetPosition(
            Editor::ETransformHelper::GetTransformFromMatrix(startMatrix * glm::inverse(curve->GetModelMatrix())).Position
        );
        break;
    }
    case Selection::END:
    {
        curve->GetEndPositionObject()->SetPosition(
            Editor::ETransformHelper::GetTransformFromMatrix(endMatrix * glm::inverse(curve->GetModelMatrix())).Position
        );
        break;
    }
    case Selection::CTRL1:
    {
        curve->GetControllPoint1Object()->SetPosition(
            Editor::ETransformHelper::GetTransformFromMatrix(controll1Matrix * glm::inverse(curve->GetModelMatrix())).Position
        );
        break;
    }
    case Selection::CTRL2:
    {
        curve->GetControllPoint2Object()->SetPosition(
            Editor::ETransformHelper::GetTransformFromMatrix(controll2Matrix * glm::inverse(curve->GetModelMatrix())).Position
        );
        break;
    }
    case Selection::NONE: break;
    };

    curve->RegenMesh();
    UpdateCurrentSegment(curve);

    bool result = fLastSelection != Selection::NONE && changedPoint == Selection::NONE;
    fLastSelection = changedPoint;
    return result;
}

EString EBezierEditTool::sGetName()
{
    return "BEZIER_EDIT";
}

EString EBezierEditTool::GetIcon() const
{
    return ICON_MD_ROUNDED_CORNER;
}


void EBezierEditTool::OnFinished(EDataBase::Entity entity)
{
    if (entity)
    {
        shared::SetValue(entity, GetComponentIdentifer(), fCurrentSegment);
    }
}

EBezierEditTool::Selection EBezierEditTool::HandleManipulate(EMat4& startMatrix, EMat4& endMatrix, EMat4& controll1Matrix, EMat4& controll2Matrix)
{
    EMat4 viewMatrix = GetViewport()->GetCamera().GetViewMatrix();
    EMat4 projectionMatrix = GetViewport()->GetCamera().GetProjectionMatrix(GetViewport()->GetWidth(), GetViewport()->GetHeight());

    if (fLastSelection == Selection::NONE || fLastSelection == Selection::START)
    {
        ImGui::PushID(420001);
        ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix), ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::LOCAL, glm::value_ptr(startMatrix));
        ImGui::PopID();

        if (ImGuizmo::IsUsing())
        {
            return Selection::START;
        }
    }


    if (fLastSelection == Selection::NONE || fLastSelection == Selection::END)
    {
        ImGui::PushID(420002);
        ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix), ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::LOCAL, glm::value_ptr(endMatrix));
        ImGui::PopID();

        if (ImGuizmo::IsUsing())
        {
            return Selection::END;
        }
    }


    if (fLastSelection == Selection::NONE || fLastSelection == Selection::CTRL1)
    {
        ImGui::PushID(420003);
        ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix), ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::LOCAL, glm::value_ptr(controll1Matrix));
        ImGui::PopID();

        if (ImGuizmo::IsUsing())
        {
            return Selection::CTRL1;
        }
    }


    if (fLastSelection == Selection::NONE || fLastSelection == Selection::CTRL2)
    {
        ImGui::PushID(420004);
        ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix), ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::LOCAL, glm::value_ptr(controll2Matrix));
        ImGui::PopID();

        if (ImGuizmo::IsUsing())
        {
            return Selection::CTRL2;
        }
    }

    return Selection::NONE;
}

void EBezierEditTool::UpdateCurrentSegment(Renderer::RBezierCurve* curve)
{
    fCurrentSegment.Start = curve->GetStartPos();
    fCurrentSegment.End = curve->GetEndPos();
    fCurrentSegment.Controll1 = curve->GetControll1();
    fCurrentSegment.Controll2 = curve->GetControll2();
}

// Transform tool

ETransformTool::ETransformTool()
    : EViewportTool(sGetName(), "ETransform"), fWasUsing(false)
{
}

bool ETransformTool::OnRender()
{
    float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };

    Renderer::RObject* attachedObject = fViewport->GetSelectionContext().GetSelectedObject();
    if (!attachedObject) { return false; }

    EMat4 viewMatrix = GetViewport()->GetCamera().GetViewMatrix();
    EMat4 projectionMatrix = GetViewport()->GetCamera().GetProjectionMatrix(GetViewport()->GetWidth(), GetViewport()->GetHeight());
    EMat4 transformMatrix = attachedObject->GetModelMatrix();
    ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix), ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::LOCAL, glm::value_ptr(transformMatrix), NULL, NULL, bounds, NULL);
    
    bool finished = false;

    if (fWasUsing && !ImGuizmo::IsUsing())
    {
        fWasUsing = false;
        finished = true;
        if (fOnChange)
        {
            fOnChange(Editor::ETransform(fLastPosition, fLastRotation, fLastScale));
        }
    }

    if (ImGuizmo::IsUsing())
    {
        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transformMatrix), glm::value_ptr(fLastPosition), glm::value_ptr(fLastRotation), glm::value_ptr(fLastScale));
        fLastRotation = EVec3(glm::radians(fLastRotation.x), glm::radians(fLastRotation.y), glm::radians(fLastRotation.z));


        attachedObject->SetPosition(fLastPosition);
        attachedObject->SetRotation(fLastRotation);
        
        attachedObject->SetScale(fLastScale);
        fWasUsing = true;
    }

    return finished;
}


Editor::ETransform ETransformTool::GetTransform() const
{
    return Editor::ETransform(fLastPosition, fLastRotation, fLastScale);
}


void ETransformTool::SetOnChange(TransformUpdateFunction func)
{
    fOnChange = func;
}

EString ETransformTool::sGetName()
{
    return "TRANSFORM_TOOL";
}

EString ETransformTool::GetIcon() const
{
    return ICON_MD_3D_ROTATION;
}

void ETransformTool::OnFinished(EDataBase::Entity entity)
{
    if (entity)
    {
        Editor::ETransform transform = GetTransform();
        shared::SetValue(entity, GetComponentIdentifer(), transform);
    }
}
