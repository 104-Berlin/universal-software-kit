#pragma once

namespace Engine {

    class EUIViewport;
    
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

        // Entity object maps
        EUnorderedMap<EDataBase::Entity, Renderer::RObject*> fEntityObjectMap;
        EUnorderedMap<Renderer::RObject*, EDataBase::Entity> fObjectEntityMap;

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

        void PushToEntityObjectMap(EDataBase::Entity entity, Renderer::RObject* object);
        void RemoveFromEntityObjectMap(EDataBase::Entity entity);
        void RemoveFromEntityObjectMap(Renderer::RObject* object);
        EDataBase::Entity GetEntityFromObject(Renderer::RObject* object) const;
        Renderer::RObject* GetObjectFromEntity(EDataBase::Entity entity) const;

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