#include "editor_extension.h"

using namespace Engine;

/***
 * 
 * #####################################################################################################################
 * CAMERA CONTROLS
 * 
 */
EUICameraControls::EUICameraControls(Renderer::RCamera* camera)
    : fCamera(camera)
{
    fCamera->SetMaxPitch(M_PI / 2);
    fCamera->SetMinPitch(-M_PI / 2);
}


EUIBasic3DCameraControls::EUIBasic3DCameraControls(Renderer::RCamera* camera, Basic3DCameraControlsSettings initialSettings)
    : EUICameraControls(camera), fSettings(initialSettings), fTarget(), fDistance(1.0f), fPinchEnabled(false), fDragPlaneEnabled(false), fMoveUpDownEnabled(false)
{

}

void EUIBasic3DCameraControls::OnMouseDrag(const events::EMouseDragEvent& event)
{
    if (event.MouseButton == 0)
    {
        if (fPinchEnabled)
        {
            fTarget += fCamera->GetRight() * -event.MouseDelta.x * fSettings.MoveSpeed;
            fTarget += fCamera->GetUp() * event.MouseDelta.y * fSettings.MoveSpeed;

            SetCameraToDistance();
        }
        else if (fDragPlaneEnabled)
        {
            float rotation = fCamera->GetYaw();

            EVec3 forward = EVec3(1, 0, 0);
            EVec3 right = EVec3(0, 0, 1);
            
            EQuat rotationQuat = EQuat(EVec3(0.0f, rotation, 0.0f));

            forward = rotationQuat * forward;
            right = rotationQuat * right;

            glm::normalize(forward);
            glm::normalize(right);

            fTarget += forward * event.MouseDelta.x * fSettings.MoveSpeed;
            fTarget += right * event.MouseDelta.y * fSettings.MoveSpeed;

            SetCameraToDistance();
        }
        else if (fMoveUpDownEnabled)
        {
            fTarget.y += event.MouseDelta.y * fSettings.MoveSpeed;

            SetCameraToDistance();
        }
        else
        {
            fCamera->TurnRight(-event.MouseDelta.x * fSettings.RotateSpeed);
            fCamera->TurnUp(-event.MouseDelta.y * fSettings.RotateSpeed);

            SetCameraToDistance();
        }
    }
}

void EUIBasic3DCameraControls::OnMouseScroll(const events::EMouseScrollEvent& event)
{
    //fDistance = fDistance * (1.0f - event.ScrollX * fSettings.ZoomSpeed);
    fDistance -= event.ScrollX * fSettings.ZoomSpeed;
    fCamera->SetZoom(fDistance);

    SetCameraToDistance();
}

void EUIBasic3DCameraControls::OnKeyDown(const events::EKeyDownEvent& event)
{
    if (event.KeyCode == ImGuiKey_Space)
    {
        fPinchEnabled = true;
    }
    if (event.Ctrl)
    {
        fDragPlaneEnabled = true;
    }
    if (event.Alt)
    {
        fMoveUpDownEnabled = true;
    }
}

void EUIBasic3DCameraControls::OnKeyUp(const events::EKeyUpEvent& event)
{
    if (event.KeyCode == ImGuiKey_Space)
    {
        fPinchEnabled = false;
    }
    if (!event.Ctrl)
    {
        fDragPlaneEnabled = false;
    }
    if (!event.Alt)
    {
        fMoveUpDownEnabled = false;
    }
}


void EUIBasic3DCameraControls::SetCameraToDistance()
{
    fCamera->SetPosition(fTarget);
    fCamera->MoveForward(-fDistance);
}

// #####################################################################################################################
// 2D Camera Controls
// #####################################################################################################################


EUIBasic2DCameraControls::EUIBasic2DCameraControls(Renderer::RCamera* camera, Basic2DCameraControlsSettings initialSettings)
    : EUICameraControls(camera), fSettings(initialSettings), fDragEnabled(false), fDistance(1.0f)
{

}

void EUIBasic2DCameraControls::OnMouseDrag(const events::EMouseDragEvent& event)
{
    if (event.MouseButton == 0)
    {
        if (fDragEnabled)
        {
            fCamera->MoveRight((-event.MouseDelta.x * fDistance) * fSettings.MoveSpeed);
            fCamera->MoveUp((event.MouseDelta.y * fDistance) * fSettings.MoveSpeed);
        }
    }
}

void EUIBasic2DCameraControls::OnMouseScroll(const events::EMouseScrollEvent& event)
{
    fDistance -= event.ScrollX * fSettings.ZoomSpeed;
    if (fDistance < 0.0f)
    {
        fDistance = 0.0f;
    }
    EVec3 cameraPos = fCamera->GetPosition();
    fCamera->SetPosition({cameraPos.x, cameraPos.y, -fDistance});
    fCamera->SetZoom(fDistance);
}

void EUIBasic2DCameraControls::OnKeyDown(const events::EKeyDownEvent& event)
{
    if (event.KeyCode == ImGuiKey_Space)
    {
        fDragEnabled = true;
    }
}

void EUIBasic2DCameraControls::OnKeyUp(const events::EKeyUpEvent& event)
{
    if (event.KeyCode == ImGuiKey_Space)
    {
        fDragEnabled = false;
    }
}
