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

        void ViewportClicked(const EVec2& screenPos, const EVec3& worldPos);
        void ActivateTool();
    protected:
        // Return true if edit was completed
        virtual bool OnRender() = 0;
        virtual void OnViewportClicked(const EVec2& screenPos, const EVec3& worldPos) {};
        virtual void OnActivateTool() {}
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
        enum class Selection
        {
            NONE,
            START,
            END
        };

        enum class EditState
        {
            CREATING,
            EDITING
        };
    private:
        Renderer::RLine* fLine;
        Selection fCurrentSelection;

        EditState fEditState;
    public:
        ELineEditTool();

        void SetLine(Renderer::RLine* line);
        Renderer::RLine* GetLine() const;

        virtual bool OnRender();
        virtual void OnViewportClicked(const EVec2& screenPos, const EVec3& worldPos);
        virtual void OnActivateTool();
        
        static EString sGetName();
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

        static EString sGetName();
    };


    namespace events {
        E_STORAGE_STRUCT(EViewportToolFinishEvent,
            (EString, ToolName)
        )
    }

}