#pragma once

namespace Engine {

    using EUIRegister = EExtensionRegister<ERef<EUIPanel>>;



    struct EAppInit
    {
        EUIRegister* PanelRegister;
    };
}