#pragma once

namespace Engine {

    class E_EDEXAPI EUICameraControls
    {
    protected:
        Renderer::RCamera* fCamera;
    public:
        EUICameraControls(Renderer::RCamera* camera);

        virtual void OnMouseDrag(const events::EMouseDragEvent& event) {}
        virtual void OnMouseScroll(const events::EMouseScrollEvent& event) {}
        virtual void OnKeyDown(const events::EKeyDownEvent& event) {}
        virtual void OnKeyUp(const events::EKeyUpEvent& event) {}
    };

    E_STORAGE_STRUCT(Basic3DCameraControlsSettings,
        (float, MoveSpeed, 0.01f),
        (float, RotateSpeed, 0.01f),
        (float, ZoomSpeed, 0.1f)
    );

    class E_EDEXAPI EUIBasic3DCameraControls : public EUICameraControls
    {
    private:
        Basic3DCameraControlsSettings fSettings;
        EVec3                         fTarget;
        double                        fDistance;
        bool                          fPinchEnabled;
        bool                          fDragPlaneEnabled;
        bool                          fMoveUpDownEnabled;
    public:
        EUIBasic3DCameraControls(Renderer::RCamera* camera, Basic3DCameraControlsSettings initialSettings = Basic3DCameraControlsSettings());
        virtual void OnMouseDrag(const events::EMouseDragEvent& event);
        virtual void OnMouseScroll(const events::EMouseScrollEvent& event);
        virtual void OnKeyDown(const events::EKeyDownEvent& event);
        virtual void OnKeyUp(const events::EKeyUpEvent& event);

    private:
        void SetCameraToDistance();
    };

    E_STORAGE_STRUCT(Basic2DCameraControlsSettings,
        (float, MoveSpeed, 0.01f),
        (float, RotateSpeed, 0.01f),
        (float, ZoomSpeed, 0.1f)
    )

    class E_EDEXAPI EUIBasic2DCameraControls : public EUICameraControls
    {
    private:
        Basic2DCameraControlsSettings fSettings;
        float                         fDistance;
        bool                          fDragEnabled;
    public:
        EUIBasic2DCameraControls(Renderer::RCamera* camera, Basic2DCameraControlsSettings initialSettings = Basic2DCameraControlsSettings());
        virtual void OnMouseDrag(const events::EMouseDragEvent& event);
        virtual void OnMouseScroll(const events::EMouseScrollEvent& event);
        virtual void OnKeyDown(const events::EKeyDownEvent& event);
        virtual void OnKeyUp(const events::EKeyUpEvent& event);
    };

}