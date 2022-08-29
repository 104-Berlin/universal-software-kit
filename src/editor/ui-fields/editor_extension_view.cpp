#include "editor.h"

using namespace Editor;
using namespace Engine;

EExtensionView::EExtensionView()
    : EUIField("EXTENSION_VIEW")
{
    ERef<EUITable> table = EMakeRef<EUITable>();
    EWeakRef<EUITable> weakTable = table;
    table->SetHeaderCells({"Name", "Loaded", "Auto Load"});

    shared::Events().Connect<events::EExtensionLoadedEvent>([this, weakTable](events::EExtensionLoadedEvent event){
        if (weakTable.expired()) { return; }
        EString extensionName = event.Extension;
        EExtension* extension = shared::ExtensionManager().GetExtension(extensionName);
        if (!extension) { return; }
        EString extensionFullPath = extension->GetFilePath();

        EExtensionViewItem item;
        item.Name = extensionName; 
        item.FullPath = extensionFullPath;
        item.Loaded = true;
        item.AutoLoad = extension->GetAutoLoad();

        // Fin extension view item
        for (size_t i = 0; i < fExtensions.size(); i++)
        {
            if (fExtensions[i].Name == extensionName)
            {
                fExtensions[i] = item;
                // Get the loaded checkbox and set it to true
                ERef<EUITableRow> row = GetRowFromIndex(i);
                if (row)
                {
                    EWeakRef<EUIField> checkbox = row->GetChildAt(2);
                    if (!checkbox.expired() && checkbox.lock())
                    {
                        std::dynamic_pointer_cast<EUICheckbox>(checkbox.lock())->SetValue(true);
                    }
                }
                return;
            }
        }
        fExtensions.push_back(item);
        weakTable.lock()->AddChild(CreateExtensionViewRow(fExtensions.size() - 1));
        
        
    }, this);

    shared::Events().Connect<events::EExtensionUnloadEvent>([this](events::EExtensionUnloadEvent e){
         // Fin extension view item
        for (size_t i = 0; i < fExtensions.size(); i++)
        {
            if (fExtensions[i].Name == e.ExtensionName)
            {
                fExtensions[i].Loaded = false;
                // Get the loaded checkbox and set it to false
                ERef<EUITableRow> row = GetRowFromIndex(i);
                if (row)
                {
                    EWeakRef<EUIField> checkbox = row->GetChildAt(2);
                    if (!checkbox.expired() && checkbox.lock())
                    {
                        std::dynamic_pointer_cast<EUICheckbox>(checkbox.lock())->SetValue(true);
                    }
                }
                return;
            }
        }
    }, this);

    ERef<EUIButton> loadExtensionButton = EMakeRef<EUIButton>("Load Extension");
    loadExtensionButton->AddEventListener<events::EButtonEvent>([](){
        EVector<EString> loadingPaths = Graphics::Wrapper::OpenFileDialog("Load Extension", {"uex"});
        for (const EString& extPath : loadingPaths)
        {
            shared::ExtensionManager().LoadExtension(extPath);
        }
    });

    AddChild(table);
    AddChild(loadExtensionButton);
}

ERef<EUITableRow> EExtensionView::CreateExtensionViewRow(size_t index)
{
    if (fExtensions.size() <= index) { return nullptr; }
    EExtensionViewItem& item = fExtensions[index];

    ERef<EUITableRow> row = EMakeRef<EUITableRow>();
    ERef<EUILabel> name = EMakeRef<EUILabel>(item.Name);
    ERef<EUICheckbox> loaded = EMakeRef<EUICheckbox>("Loaded");
    loaded->SetLabelVisible(false);
    loaded->SetValue(item.Loaded);
    ERef<EUICheckbox> autoLoad = EMakeRef<EUICheckbox>("Auto Loaded");
    autoLoad->SetValue(item.AutoLoad);
    autoLoad->SetLabelVisible(false);

    loaded->AddEventListener<events::ECheckboxEvent>([this, item, index](events::ECheckboxEvent event){
        if (event.Checked)
        {
            shared::ExtensionManager().LoadExtension(item.FullPath);
        }
        else
        {
            EExtension* extension = shared::ExtensionManager().GetExtension(item.Name);
            if (!extension) { return; }

            shared::ExtensionManager().UnloadExtension(extension);
        }
        if (index < fExtensions.size())
        {
            fExtensions[index].Loaded = event.Checked;
        }
    });

    autoLoad->AddEventListener<events::ECheckboxEvent>([this, item, index](events::ECheckboxEvent event){
        shared::ExtensionManager().SetExtensionAutoLoad(item.Name, event.Checked);
        if (index < fExtensions.size())
        {
            fExtensions[index].AutoLoad = event.Checked;
        }
    });



    row->AddChild(name);
    row->AddChild(loaded);
    row->AddChild(autoLoad);

    return row;
}

ERef<Engine::EUITableRow> EExtensionView::GetRowFromIndex(size_t index)
{
    EWeakRef<EUIField> table = GetChildAt(0);
    if (!table.expired() && table.lock())
    {
        EWeakRef<EUIField> tableRow = table.lock()->GetChildAt(index);
        if (!tableRow.expired() && tableRow.lock())
        {
            return std::dynamic_pointer_cast<EUITableRow>(tableRow.lock());
        }
    }
    return nullptr;
}