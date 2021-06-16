#pragma once

namespace Engine {

#define E_NOT_IMPLEMENTED { E_WARN("Register type is not implemented for " + fName); }

    class E_API EScriptContext
    {
    private:
        EString fName;
    public:
        EScriptContext(const EString& name);
        virtual ~EScriptContext() = default;

        virtual void RegisterType(const EValueDescription& valueDescription) E_NOT_IMPLEMENTED

        virtual void Execute(const EString& command) E_NOT_IMPLEMENTED
    };

}