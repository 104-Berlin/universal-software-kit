#pragma once

#include "editor_basic_object_view.h"
namespace Editor {

    class EExtensionView : public Engine::EUIField
    {
    public:
        EExtensionView();
        virtual ~EExtensionView();

        virtual bool OnRender() override;
    };

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

        virtual bool OnRender() override;
    };

    class ECommandLine : public Engine::EUIField
    {
    public:
        ECommandLine();

        virtual bool OnRender() override;
    };

}