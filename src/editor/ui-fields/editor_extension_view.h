#pragma once

namespace Editor {

    E_STORAGE_STRUCT(EExtensionViewItem,
        (EString, Name),
        (EString, FullPath),
        (bool,    Loaded),
        (bool,    AutoLoad)
    )

    class EExtensionView : public Engine::EUIField
    {
    private:
        EVector<EExtensionViewItem> fExtensions;
    public: 
        EExtensionView();

    private:
        ERef<Engine::EUITableRow> CreateExtensionViewRow(size_t index);
        ERef<Engine::EUITableRow> GetRowFromIndex(size_t index);
    };

}