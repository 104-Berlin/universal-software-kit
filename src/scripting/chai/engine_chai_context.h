#pragma once

namespace Engine {

    class E_API EChaiContext : public EScriptContext
    {
    private:
        chaiscript::ChaiScript fContext;
    public:
        EChaiContext();

        virtual void Execute(const EString& command);
    };
    

}