#pragma once

namespace Editor {

    E_STORAGE_STRUCT(EExtensionViewItem,
        (EString, Name),
        (bool,    Loaded),
        (bool,    AutoLoad)
    )

    class EExtensionView : public Engine::EUIField
    {
    private:
        EVector<EExtensionViewItem> fExtensions;
    public: 
        EExtensionView();
    };

}