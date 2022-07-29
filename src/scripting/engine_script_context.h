#pragma once

namespace Engine {

#define E_NOT_IMPLEMENTED { E_WARN("Not implemented for " + fName); }


    class E_SAPI EScriptContext
    {
        using Function = std::function<void(const EWeakRef<EProperty>&)>;
    private:
        EString fName;
        EUnorderedMap<EString, EBaseTask*> fScripts;
    public:
        EScriptContext(const EString& name);
        virtual ~EScriptContext() = default;

        virtual void RegisterTask(EBaseTask* task);
        virtual void Execute(const EString& command) E_NOT_IMPLEMENTED
    protected:
        EBaseTask* GetTask(const EString& name);
        ERef<EProperty> RunTask(const EString& taskName, EWeakRef<EProperty> inValue);
    };

}