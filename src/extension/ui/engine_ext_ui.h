#pragma once


namespace Engine {

    namespace intern {
        E_EXTAPI void InitUI();
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


    /**
     * Most Basic UI Element
     */
    class E_EXTAPI EUIField
    {
        using UpdateFunction = std::function<void()>;
    private:
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
    public:
        EUIField(const EString& label);

        /**
         * Adds a child to the list
         * @param child The UI Field thats gets added to the list
         */
        void AddChild(const ERef<EUIField>& child);

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
         * Returns the current label of the field
         * @return The current label
         */
        const EString& GetLabel() const;

        /**
         * Adds a listener to specified EventType
         * @param EventType typename of one of the UI Event structs
         * @param callbackFunction Function which will be invoked with given EventType when event is triggered
         */
        template <typename EventType, typename CB>
        void AddEventListener(CB&& callbackFunction)
        {
            fEventDispatcher.Connect<EventType>(callbackFunction);
        }
    };

    class E_EXTAPI EUIPanel : public EUIField
    {
    private:
        /**
         * Wether the panel is open.
         */
        bool fOpen;
        /**
         * This is true when the panel was closed this frame. This is to remove an imgui bug
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
    private:
        bool Impl_Render();
        void Impl_RenderEnd();
    };

    class E_EXTAPI EUIViewport : public EUIField
    {
    public:
        EUIViewport();


        virtual bool OnRender() override;
#ifdef EXT_RENDERER_ENABLED
    private:
        /**
         * Render function which will be called with the viewport size
         */
        using RenderFunction = std::function<void(Graphics::GContext*, Graphics::GFrameBuffer*)>;

        RenderFunction fRenderFuntion;
        UIImpl::EImGuiViewport fImGuiViewport;
    public:
        void SetRenderFunction(RenderFunction renderFunction);
#endif
    };

}