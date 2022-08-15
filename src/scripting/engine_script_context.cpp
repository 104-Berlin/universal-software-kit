#include "prefix_scripting.h"


using namespace Engine;

EScriptContext::EScriptContext(const EString& name) 
    : fName(name)
{
    
}

void EScriptContext::RegisterTask(EBaseTask* task)
{
    fScripts[task->GetName()] = task;
}


EBaseTask* EScriptContext::GetTask(const EString& taskName)
{
    auto it = fScripts.find(taskName);
    if (it == fScripts.end())
    {
        return nullptr;
    }
    return it->second;
}

ERef<EProperty> EScriptContext::RunTask(const EString& taskName, EWeakRef<EProperty> inValue)
{
    auto task = GetTask(taskName);
    if (task)
    {
        return task->Execute(inValue);
    }
    return nullptr;
}
