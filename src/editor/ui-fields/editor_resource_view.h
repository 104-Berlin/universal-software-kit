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
        Engine::EResourceData::t_ID selectedResource;
        EVector<Resource>           fResources;
    public:
        EResourceView();
    };
}