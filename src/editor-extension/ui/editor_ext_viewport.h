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
        (float, MoveSpeed),
        (float, RotateSpeed),
        (float, ZoomSpeed)
    );

    class E_EDEXAPI EUIBasic3DCameraControls : public EUICameraControls
    {
    private:
        Basic3DCameraControlsSettings fSettings;
        EVec3                         fTarget;
        double                        fDistance;
        double                        fCameraPitch;
        double                        fCameraYaw;
        bool                          fPinchEnabled;
        bool                          fDragPlaneEnabled;
        bool                          fMoveUpDownEnabled;
    public:
        EUIBasic3DCameraControls(Renderer::RCamera* camera, Basic3DCameraControlsSettings initialSettings = Basic3DCameraControlsSettings(0.1f, 0.01f, 0.1f));
        virtual void OnMouseDrag(const events::EMouseDragEvent& event);
        virtual void OnMouseScroll(const events::EMouseScrollEvent& event);
        virtual void OnKeyDown(const events::EKeyDownEvent& event);
        virtual void OnKeyUp(const events::EKeyUpEvent& event);
    };

    class E_EDEXAPI EUIViewport : public EUIField
    {
    public:
        enum class ViewType
        {
            DIFFUSE,
            NORMAL,
            DEPTH
        };
    private:
        EVector<EViewportTool*> fRegisteredTools;
        EViewportTool*          fActiveTool;
        ViewType                fViewType;
    public:
        EUIViewport(const Renderer::RCamera& = Renderer::RCamera(Renderer::ECameraMode::ORTHOGRAPHIC));
        virtual ~EUIViewport();

        virtual bool OnRender() override;
    private:
        Graphics::GFrameBuffer* fFrameBuffer;

        Renderer::RRenderer3D fRenderer;
        Renderer::RCamera fCamera;
        Renderer::RScene fScene;

        EUICameraControls* fCameraControls;
    public:
        Renderer::RScene& GetScene();
        const Renderer::RScene& GetScene() const;

        const Renderer::RCamera& GetCamera() const;
        Renderer::RCamera& GetCamera();

        EViewportTool* AddTool(EViewportTool* newTool);

        void SetViewType(ViewType type);

        EVec2 Project(const EVec3& point) const;
        EVec3 Unproject(const EVec3& point) const;

        EVector<EViewportTool*> GetRegisteredTools();
        EViewportTool* GetActiveTool();
        void SetActiveTool(const EString& toolName);
    };

    class E_EDEXAPI EUIViewportToolbar : public EUIField
    {
        EWeakRef<EUIViewport> fViewport;
    public:
        EUIViewportToolbar(EWeakRef<EUIViewport> viewport);

        virtual bool OnRender() override;
    private:
        void Regenerate();
    };


}