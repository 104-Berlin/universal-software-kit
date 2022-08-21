#pragma once

namespace Engine {

    E_STORAGE_ENUM(EViewportType,
        DEFAULT_3D,
        DEFAULT_2D,
        FRONT_RIGHT_TOP_3D
    )

    E_STORAGE_STRUCT(EViewportDescription,
        (EString, Name), // Name of the viewport
        (EString, ExtensionName),
        (EViewportType, Type)
    )

    using TRenderFunction = std::function<void(Renderer::RObject*, ERef<EProperty>)>;
    using TInitViewportToolsFunction = std::function<EVector<EViewportTool*>()>;
    using TViewportToolFinishFunction = std::function<void(events::EViewportToolFinishEvent, EWeakRef<EUIViewport>)>;

    struct EUIViewportRenderFunction
    {
        EViewportDescription                            Description;
        EValueDescription                               ValueDescription;
        TRenderFunction                                 RenderFunction;
        TInitViewportToolsFunction                      InitViewportTools;
        TViewportToolFinishFunction                     ToolFinished;

        bool                                            NeedsOwnObject = true;
        EUnorderedMap<EDataBase::Entity, size_t>        ComponentObjectIndex; // Dont change this!!
    };

    using EViewportRenderFunctionRegister = EExtensionRegister<EUIViewportRenderFunction>;

    using TViewportTypeMap = EUnorderedMap<EViewportType::opts, EWeakRef<EUIField>>;
    using TRenderFunctionMap = EUnorderedMap<EViewportType::opts, EUnorderedMap<EValueDescription::t_ID, EUIViewportRenderFunction>>;
    class E_EDEXAPI EUIViewportManager
    {
        using TViewportFunction = std::function<void(ERef<EUIViewport>)>;
    private:
        TViewportTypeMap                    fViewports;
        TRenderFunctionMap                  fRenderFunctions;
        EUIRegister*                        fUIRegister;
        EViewportRenderFunctionRegister*    fViewportRenderFunctionRegister;
    public:
        public:
        EUIViewportManager(EUIRegister* uiRegister, EViewportRenderFunctionRegister* renderFunctionRegister);
        
        
        void ReloadViewports();
    private:
        ERef<EUIField> CreateViewport(const EViewportType& type) const;

        void HandleEntityChange(EntityChangeEvent event);
        void ViewportToolFinished(events::EViewportToolFinishEvent event, EViewportType viewportType);

        void CallRenderFunctionForComponent(EDataBase::Entity entity, EWeakRef<EProperty> component, EViewportType type);

        void EachViewport(EViewportType type, const TViewportFunction& func);
    };

}