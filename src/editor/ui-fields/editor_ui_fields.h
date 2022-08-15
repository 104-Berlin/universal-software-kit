#pragma once

#include "editor_basic_object_view.h"
#include "editor_resource_view.h"
#include "editor_basic_register_view.h"
#include "editor_extension_view.h"

namespace Editor {

    class ECommandLine : public Engine::EUIField
    {
    private:
        Engine::EScriptContext* fScriptContext;
    public:
        ECommandLine(Engine::EScriptContext* scriptContext);

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