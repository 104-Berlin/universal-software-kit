#include "engine_extension.h"

using namespace Engine;

EBaseTask::EBaseTask(const EString& name, u16 type) 
    : fName(name), fType(type)
{
    
}

EBaseTask::~EBaseTask() 
{
    
}

EStructProperty* EBaseTask::Execute(EStructProperty* inValue) 
{
    return OnExecute(inValue);
}


ECFuncTask::ECFuncTask(const EString& name) 
    : EBaseTask(name, ETaskType_SINGLE_EXECUTION)
{
    
}

ECFuncTask::ECFuncTask(const EString& name, CFunc_Param_Return func) 
    : ECFuncTask(name)
{
    SetFunc(func);
}

ECFuncTask::ECFuncTask(const EString& name, CFunc_Param_NoReturn func) 
    : ECFuncTask(name)
{
    SetFunc(func);   
}

ECFuncTask::ECFuncTask(const EString& name, CFunc_NoParam_Return func) 
    : ECFuncTask(name)
{
    SetFunc(func);
}

ECFuncTask::ECFuncTask(const EString& name, CFunc_NoParam_NoReturn func) 
    : ECFuncTask(name)
{
    SetFunc(func);
}

ECFuncTask::~ECFuncTask() 
{
    
}

EStructProperty* ECFuncTask::OnExecute(EStructProperty* inValue) 
{
    if (!fExecuteFunction)
    {
        E_ERROR("ECFuncTask::OnExecute: no function set");
        return nullptr;
    }

    return fExecuteFunction(inValue);
}

void ECFuncTask::SetFunc(CFunc_NoParam_NoReturn func) 
{
    fExecuteFunction = [func](EStructProperty* inValue) -> EStructProperty*
    {
        func();
        return nullptr;
    };
}

void ECFuncTask::SetFunc(CFunc_NoParam_Return func) 
{
    fExecuteFunction = [func](EStructProperty* inValue) -> EStructProperty*
    {
        return func();
    };
}

void ECFuncTask::SetFunc(CFunc_Param_NoReturn func) 
{
    fExecuteFunction = [func](EStructProperty* inValue) -> EStructProperty*
    {
        func(inValue);
        return nullptr;
    };
}

void ECFuncTask::SetFunc(CFunc_Param_Return func) 
{
    fExecuteFunction = func;
}