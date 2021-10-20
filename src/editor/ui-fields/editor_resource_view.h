#pragma once

namespace Editor {
    class EResourceView : public Engine::EUIField
    {
        struct ResourceType
        {
            EString TypeName;
            bool    CanCreate;
        };
        struct Resource
        {
            Engine::EResourceData::t_ID ID;
            EString                     Name;
            EString                     Path;
        };
    private:
        EUnorderedMap<EString, EVector<Resource>>    fResources;
        EUIField*                                         fLastSelected;

        float                                             fPreviewSize;
        ResourceType                                      fSelectedResourceType;
    public:
        EResourceView();
    };
}