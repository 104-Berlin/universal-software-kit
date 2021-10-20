#pragma once

namespace Engine {

    class EUIViewport;
    
    class E_EDEXAPI EViewportTool 
    {
    private:
        bool fVisible;
        EString fToolName;
        EUIViewport* fViewport;
    public:
        EViewportTool(const EString& toolName);
        virtual ~EViewportTool() = default;

        bool Render();

        bool IsVisible() const;
        void SetVisible(bool visible);

        const EString& GetToolName() const;

        virtual EString GetIcon() const;

        void SetViewport(EUIViewport* viewport);
        EUIViewport* GetViewport() const;
    protected:
        // Return true if edit was completed
        virtual bool OnRender() = 0;
    };



    class E_EDEXAPI EPointMoveTool : public EViewportTool
    {
    private:
        EVec2 fCenterPosition;
    public:
        EPointMoveTool();

        virtual bool OnRender() override;
    };

    class E_EDEXAPI ELineEditTool : public EViewportTool
    {
        enum Selection
        {
            NONE,
            START,
            END
        };
    private:
        Renderer::RLine* fLine;
        Selection fCurrentSelection;
    public:
        ELineEditTool();

        void SetLine(Renderer::RLine* line);
        Renderer::RLine* GetLine() const;

        virtual bool OnRender();
    };

    class E_EDEXAPI EBezierEditTool : public EViewportTool
    {
        enum Selection
        {
            NONE,
            START,
            END,
            CTRL1,
            CTRL2
        };
    private:
        Selection fCurrentSelection;

        Renderer::RBezierCurve* fCurve;
    public:
        EBezierEditTool();

        virtual bool OnRender() override;

        void SetBezierCurve(Renderer::RBezierCurve* curve);
        Renderer::RBezierCurve* GetCurve() const;
    };


    namespace events {
        E_STORAGE_STRUCT(EViewportToolFinishEvent,
            (EString, ToolName)
        )
    }


    class E_EDEXAPI EUIViewport : public EUIField
    {
    private:
        EVector<EViewportTool*> fRegisteredTools;
    public:
        EUIViewport(const Renderer::RCamera& = Renderer::RCamera(Renderer::ECameraMode::ORTHOGRAPHIC));
        virtual ~EUIViewport();

        virtual bool OnRender() override;
    private:
        Graphics::GFrameBuffer* fFrameBuffer;

        Renderer::RRenderer3D fRenderer;
        Renderer::RCamera fCamera;
        Renderer::RScene fScene;
    public:
        Renderer::RScene& GetScene();
        const Renderer::RScene& GetScene() const;

        const Renderer::RCamera& GetCamera() const;
        Renderer::RCamera& GetCamera();

        EViewportTool* AddTool(EViewportTool* newTool);


        EVec2 Project(const EVec3& point) const;
        EVec3 Unproject(const EVec3& point) const;
        
        EVector<EViewportTool*> GetRegisteredTools();
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