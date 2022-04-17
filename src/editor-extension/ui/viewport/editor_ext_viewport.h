#pragma once

namespace Engine {

    class EUIViewport;

    class E_EDEXAPI EUIViewportTransformControls
    {
        using TransformUpdateFunction = std::function<void(Editor::ETransform)>;
    private:
        EUIViewport*          fViewport;
        Renderer::RObject*    fAttachedObject;
        bool                  fVisible;
        bool                  fWasUsing;
        TransformUpdateFunction fOnChange;
        
        EVec3 fLastPosition;
        EVec3 fLastRotation; 
        EVec3 fLastScale;
    public:
        EUIViewportTransformControls(EUIViewport* viewport);

        void OnRender();

        void SetAttachedObject(Renderer::RObject* object);

        void SetVisible(bool visible);
        bool IsVisible() const;
        void SetOnChange(TransformUpdateFunction func);
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
        EUIViewportTransformControls fTransformControls;
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

        float GetWidth() const;
        float GetHeight() const;

        EUIViewportTransformControls& GetTransformControls();
        const EUIViewportTransformControls& GetTransformControls() const;

        EVector<EViewportTool*> GetRegisteredTools();
        EViewportTool* GetActiveTool();
        void SetActiveTool(const EString& toolName);

        template <typename T>
        void SetCameraControls()
        {
            if (fCameraControls)
            {
                delete fCameraControls;
            }
            fCameraControls = new T(&fCamera);
        }

        template <typename T, typename U>
        void SetCameraControls(const U& settings)
        {
            if (fCameraControls)
            {
                delete fCameraControls;
            }
            fCameraControls = new T(&fCamera, settings);
        }
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