#pragma once

namespace Engine {

    class E_SAPI EChaiContext : public EScriptContext
    {
    private:
        chaiscript::ChaiScript fContext;
    public:
        EChaiContext();

        virtual void Execute(const EString& command);
    };
    

}