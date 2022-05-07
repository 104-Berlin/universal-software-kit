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

    using TViewportRenderMap = EUnorderedMap<EString, EUIViewportRenderFunction>;

    class E_EDEXAPI EUIViewportManager
    {
    private:
        TViewportRenderMap fViewportRenderMap;
    public:
        public:
        EUIViewportManager();
        void AddViewport(const EViewportDescription& description, TRenderFunction renderFunction);
    };

}