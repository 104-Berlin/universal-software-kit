#pragma once

namespace Editor {
    class EResourceView : public Engine::EUIField
    {
        struct Resource
        {
            Engine::EResourceData::t_ID ID;
            EString                     Name;
            EString                     Path;
        };
    private:
        EUnorderedMap<EString, EVector<Resource>>    fResources;
        EUIField*                  fLastSelected;

        EString                             fSelectedResourceType;
    public:
        EResourceView();
    };
}