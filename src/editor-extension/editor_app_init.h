#pragma once

namespace Engine {

    struct EAppInit
    {
        EUIRegister* PanelRegister;
        EUIValueRegister* ValueFieldRegister;
        EViewportRenderFunctionRegister* ViewportRenderFunctions;
    };

}