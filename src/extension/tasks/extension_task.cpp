#include "engine_extension.h"

using namespace Engine;

EBaseTask::EBaseTask(const EString& name, u16 type, bool hasInput, bool hasOutput) 
    : fName(name), fType(type), fHasInput(hasInput), fHasOutput(hasOutput)
{
    
}

EBaseTask::~EBaseTask() 
{
    
}

EStructProperty* EBaseTask::Execute(EStructProperty* inValue) 

{
    return OnExecute(inValue);
}

const EString& EBaseTask::GetName() const
{
    return fName;
}

bool EBaseTask::HasInput() const
{
    return fHasInput;
}

bool EBaseTask::HasOutput() const
{
    return fHasOutput;
}

const EValueDescription& EBaseTask::GetInputDescription() const
{
    return fInputDescription;
}

const EValueDescription& EBaseTask::GetOutputDescription() const
{
    return fOutputDescription;
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
    fHasInput = false;
    fHasOutput = false;
    fExecuteFunction = [func](EStructProperty* inValue) -> EStructProperty*
    {
        func();
        return nullptr;
    };
}

void ECFuncTask::SetFunc(CFunc_NoParam_Return func) 
{
    fHasInput = false;
    fHasOutput = true;
    fExecuteFunction = [func](EStructProperty* inValue) -> EStructProperty*
    {
        return func();
    };
}

void ECFuncTask::SetFunc(CFunc_Param_NoReturn func) 
{
    fHasInput = true;
    fHasOutput = false;
    fExecuteFunction = [func](EStructProperty* inValue) -> EStructProperty*
    {
        func(inValue);
        return nullptr;
    };
}

void ECFuncTask::SetFunc(CFunc_Param_Return func) 
{
    fHasInput = true;
    fHasOutput = true;
    fExecuteFunction = func;
}