#pragma once

#include "editor_basic_object_view.h"
#include "editor_resource_view.h"
#include "editor_basic_register_view.h"
#include "editor_extension_view.h"

namespace Editor {

    /*class EExtensionView : public Engine::EUIField
    {
    public:
        EExtensionView();
        virtual ~EExtensionView();

        virtual bool OnRender() override;
    };*/


    class ECommandLine : public Engine::EUIField
    {
    public:
        ECommandLine();

        virtual bool OnRender() override;
    };


    class EServerStatusUI : public Engine::EUIField
    {
    private:
        int fCurrentPort;
    public:
        EServerStatusUI();

        virtual bool OnRender() override;
    };  

}