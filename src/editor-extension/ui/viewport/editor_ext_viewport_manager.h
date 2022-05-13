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

    struct EUIViewportRenderFunction
    {
        EViewportDescription                            Description;
        EValueDescription                               ValueDescription;
        TRenderFunction                                 RenderFunction;

        bool                                            NeedsOwnObject = true;
        EUnorderedMap<EValueDescription::t_ID, size_t>  ComponentObjectIndex; // Dont change this!!
    };

    using EViewportRenderFunctionRegister = EExtensionRegister<EUIViewportRenderFunction>;

    using TViewportTypeMap = EUnorderedMap<EViewportType::opts, EWeakRef<EUIViewport>>;
    using TRenderFunctionMap = EUnorderedMap<EViewportType::opts, EUnorderedMap<EValueDescription::t_ID, EUIViewportRenderFunction>>;
    class E_EDEXAPI EUIViewportManager
    {
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
        ERef<EUIViewport> CreateViewport(const EViewportType& type) const;

        void HandleEntityChange(EntityChangeEvent event);
    };

}