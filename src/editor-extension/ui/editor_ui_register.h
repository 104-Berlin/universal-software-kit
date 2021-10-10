#pragma once

namespace Engine {

    using EUIRegister = EExtensionRegister<ERef<EUIPanel>>;

    using EUIValueRegisterEntry = EPair<EValueDescription::t_ID, std::function<ERef<EUIField>(EProperty*)>>;
    using EUIValueRegister = EExtensionRegister<EUIValueRegisterEntry>;

    class EFindValueFieldByType
    {
    private:
        EValueDescription::t_ID fType;
    public:
        EFindValueFieldByType(const EValueDescription::t_ID& type)
            : fType(type)
        {}

        bool operator()(EUIValueRegisterEntry other) const
        {
            return other.first == fType;
        }
    };

    struct EAppInit
    {
        EUIRegister* PanelRegister;
        EUIValueRegister* ValueFieldRegister;
    };
}