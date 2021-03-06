#pragma once


namespace Engine {

    namespace intern {
        E_EDEXAPI void InitUI();
    }

    /**
     * Gets the next ui id
     * This will hopefully be unique, but at least good enough to not mess anything up
     */
    static i32 next_ui_id()
    {
        static i32 currentUiId = 0;
        return ++currentUiId; 
    }


namespace events {

    E_STORAGE_STRUCT(EMouseMoveEvent,
        (EVec2, Position),
        (EVec2, MouseDelta)
    )

    E_STORAGE_STRUCT(EMouseDownEvent,
        (EVec2,   Position),
        (u32,     MouseButton)
    )

    E_STORAGE_STRUCT(EMouseDragEvent,
        (EVec2,   Position),
        (EVec2,   MouseDelta),
        (u32,     MouseButton)
    )

}

    /**
     * Most Basic UI Element
     */
    class E_EDEXAPI EUIField
    {
        using UpdateFunction = std::function<void()>;
    protected:
        /**
         * An id that will be incremented to be unique
         */
        i32 fID;

        /**
         * The Label of the UI Element
         * Often used as display name, or to identify
         */
        EString fLabel;

        /**
         * Wether the field should be shown
         */
        bool fVisible;

        /**
         * Wether the field should be updated or not
         */
        bool fDirty;

        /**
         * Width of the field.
         * If 0 default size will be calculated.
         */
        float fWidthOverride;

        /**
         * Height of the field.
         * If 0 default size will be calculated.
         */
        float fHeightOverride;

        /**
         * Final size after rendering
         */
        EVec2 fCalculatedSize;

        /**
         * A List of all Children. These will be rendered, if the OnRender funtion returns true and this Field is visible
         */
        EVector<ERef<EUIField>> fChildren;

        /**
         * A custom function that will be called when the UI Field is dirty.
         */
        UpdateFunction fCustomUpdateFunction;

        /**
         * The EventDispatcher of this element.
         * Here all events will be queued, and you can add events via AddEventListener<EventType>(CallbackFunction)
         */
        EEventDispatcher fEventDispatcher;

        /**
         * Last mouse pos to get mouse delta when moved
         */
        EVec2 fLastMousePos;
    public:
        EUIField(const EString& label);

        /**
         * Adds a child to the list
         * @param child The UI Field thats gets added to the list
         * @return The added child
         */
        ERef<EUIField> AddChild(const ERef<EUIField>& child);

        /**
         * Removes a child from the list
         * @param child Weak pointer to field which gets deleted.
         */
        void RemoveChild(const EWeakRef<EUIField>& child);

        /**
         * Removes all childre
         */
        void Clear();

        /**
         * Sets the custom update function
         * @param UpdateFunction
         */
        void SetCustomUpdateFunction(UpdateFunction function);

        /**
         * Draws the ui field to imgui at the moment
         * This could be swapped with other ui backends maybe.
         * Some kind of HTML representation could be possible
         */
        void Render();

        /**
         * Virtual function for custom rendering behaviour.
         * @return Wether the children of this field should be rendered or not
         */
        virtual bool OnRender();

        /**
         * Virtual function to end the rendering
         * This can be usefull in some cases
         */
        virtual void OnRenderEnd();

        /**
         * This calls all registered event listener and clear the event loop
         */
        void UpdateEventDispatcher();

        /**
         * Gets called when the bevore own eventdispatcher gets updated
         */
        virtual void OnUpdateEventDispatcher() {}

        /**
         * Returns the current label of the field
         * @return The current label
         */
        const EString& GetLabel() const;

        /**
         * Sets the size of the Field.
         * @param width If 0 width will get calculated automatically!
         * @param height If 0 height will get calculated automatically!
         */
        void SetSize(float width, float height);

        /**
         * Sets the size of the Field.
         * @param size If one acis equal 0, the size for this axis will get calculated automatically!
         */
        void SetSize(const EVec2& size);


        /** Sets width of field
         * @param If 0 width will get calculated automatically!
         */
        void SetWidth(float width);

        /** Sets height of field
         * @param If 0 height will get calculated automatically!
         */
        void SetHeight(float height);

        /**
         * @return Size of the Field
         */
        EVec2 GetSize() const;

        /**
         * @return Width of the Field
         */
        float GetWidth() const;
        /**
         * @return Height of the Field
         */
        float GetHeight() const;


        /**
         * Adds a listener to specified EventType
         * @param EventType typename of one of the UI Event structs
         * @param callbackFunction Function which will be invoked with given EventType when event is triggered
         */
        template <typename EventType, typename CB>
        void AddEventListener(CB callbackFunction)
        {
            fEventDispatcher.Connect<EventType>(callbackFunction);
        }
    };

    class E_EDEXAPI EUIPanel : public EUIField
    {
    private:
        /**
         * Wether the panel is open.
         */
        bool fOpen;

        /**
         * To remove an imgui thing
         */
        bool fWasJustClosed;
    public:
        EUIPanel(const EString& title);

        virtual bool OnRender() override;
        virtual void OnRenderEnd() override;

        /**
         * @return Wether the Panel is open
         */
        bool IsOpen() const;
        /**
         * Closes the panel
         */
        void Close();
        /**
         * Shows the panel
         */
        void Open();
    };

    class E_EDEXAPI EUIViewport : public EUIField
    {
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
    };


namespace events {
    E_STORAGE_STRUCT(EButtonEvent,
        (int, a)
    )
}

    class E_EDEXAPI EUIButton : public EUIField
    {
    public:
        EUIButton(const EString& label);

        virtual bool OnRender() override;
    };

    E_STORAGE_STRUCT(ETextChangeEvent,
        (EString, Value)
    )

    class E_EDEXAPI EUITextField : public EUIField
    {
    private:
        EString fContent;
    public:
        EUITextField(const EString& label, const EString& content = "");

        virtual bool OnRender() override;
    };


    class E_EDEXAPI EUIMainMenuBar : public EUIField
    {
    private:
        bool fOpen;
    public:
        EUIMainMenuBar();

        virtual bool OnRender() override;
        virtual void OnRenderEnd() override;
        
    };

    class E_EDEXAPI EUIMenu : public EUIField
    {
    private:
        bool    fOpen;
    public:
        EUIMenu(const EString& displayName = "MenuBar");

        virtual bool OnRender() override;
        virtual void OnRenderEnd() override;
    };

    class E_EDEXAPI EUIContextMenu : public EUIField
    {
    private:
        bool        fOpen;
    public:
        EUIContextMenu(const EString& displayName = "ContextMenu");

        virtual bool OnRender() override;
        virtual void OnRenderEnd() override;
    };

    class E_EDEXAPI EUIMenuItem : public EUIField
    {
    public:
        EUIMenuItem(const EString& label);

        virtual bool OnRender() override;
    };


    class E_EDEXAPI EUIImageView : public EUIField
    {
    private:
        Graphics::GTexture2D* fTexture;
    public:
        EUIImageView();
        ~EUIImageView();

        void SetTextureData(u8* data, size_t width, size_t height);

        virtual bool OnRender() override;
    };

}