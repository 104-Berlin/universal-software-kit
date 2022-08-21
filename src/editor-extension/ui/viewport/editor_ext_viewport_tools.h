#pragma once


namespace Engine {

    class EUIViewport;
    
    class E_EDEXAPI EViewportTool 
    {
    protected:
        bool fVisible;
        EString fToolName;
        EString fComponentIdentifier;
        EUIViewport* fViewport;
    public:
        EViewportTool(const EString& toolName, const EString& componentIdentifier = "");
        virtual ~EViewportTool() = default;

        bool Render();

        bool IsVisible() const;
        void SetVisible(bool visible);

        void SetComponentIdentifier(const EString& ident);
        const EString& GetComponentIdentifer() const;

        const EString& GetToolName() const;

        virtual EString GetIcon() const;

        void SetViewport(EUIViewport* viewport);
        EUIViewport* GetViewport() const;

        void ViewportClicked(const EVec2& screenPos, const EVec3& worldPos);
        void ActivateTool();

        void Finish();

        Renderer::RObject* GetActiveObject() const;
    protected:
        // Return true if edit was completed
        virtual bool OnRender() = 0;
        virtual void OnViewportClicked(const EVec2& screenPos, const EVec3& worldPos) {};
        virtual void OnActivateTool() {}

        virtual void OnFinished(EDataBase::Entity entity) = 0;
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
        Selection fCurrentSelection;
        EditState fEditState;

        Editor::ELine fCurrentLine;
    public:
        ELineEditTool();

        virtual bool OnRender() override;
        virtual void OnViewportClicked(const EVec2& screenPos, const EVec3& worldPos) override;
        virtual void OnActivateTool() override;
        
        static EString sGetName();

        virtual EString GetIcon() const override;

        virtual void OnFinished(EDataBase::Entity entity) override;
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
        Selection fLastSelection;

        Editor::ECurveSegment fCurrentSegment;
    public:
        EBezierEditTool();

        virtual bool OnRender() override;

        static EString sGetName();

        virtual EString GetIcon() const override;

        
        virtual void OnFinished(EDataBase::Entity entity) override;
    private:
        Selection HandleManipulate(EMat4& startMatrix, EMat4& endMatrix, EMat4& controll1Matrix, EMat4& controll2Matrix);
        void UpdateCurrentSegment(Renderer::RBezierCurveEdit* curve);
    };

    class E_EDEXAPI ETransformTool : public EViewportTool
    {
        using TransformUpdateFunction = std::function<void(Editor::ETransform)>;
    private:
        bool                    fWasUsing;
        TransformUpdateFunction fOnChange;
        
        EVec3 fLastPosition;
        EVec3 fLastRotation; 
        EVec3 fLastScale;

        int   fGizmoID;
    public:
        ETransformTool();

        virtual bool OnRender() override;

        Editor::ETransform GetTransform() const;

        void SetOnChange(TransformUpdateFunction func);

        static EString sGetName();

        virtual EString GetIcon() const override;

        virtual void OnFinished(EDataBase::Entity entity) override;
    };



    namespace events {
        E_STORAGE_STRUCT(EViewportToolFinishEvent,
            (EString, ToolName)
        )
    }

}