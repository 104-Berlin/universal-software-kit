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

    using TRenderFunction = std::function<void(Renderer::RObject*)>;

    struct EUIViewportRenderFunction
    {
        EViewportDescription    Description;
        TRenderFunction         RenderFunction;
    };

    using TViewportTypeMap = EUnorderedMap<EViewportType::opts, EPair<EWeakRef<EUIViewport>, EUnorederedMap<EValueDescription::t_ID, TRenderFunction>>>;

    class E_EDEXAPI EUIViewportManager
    {
    private:
        TViewportTypeMap    fViewports;
        EUIRegister*        fUIRegister;
    public:
        public:
        EUIViewportManager(EUIRegister* uiRegister);
        void AddViewportDescription(const EViewportDescription& description, TRenderFunction renderFunction);

    private:
        ERef<EUIViewport> CreateViewport(const EViewportType& type);
    };

}