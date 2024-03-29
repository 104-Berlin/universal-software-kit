#include "engine_extension.h"

using namespace Engine;

EBaseTask::EBaseTask(const EString& name, u16 type, bool hasInput, bool hasOutput) 
    : fName(name), fType(type), fHasInput(hasInput), fHasOutput(hasOutput)
{
    
}

EBaseTask::~EBaseTask() 
{
    
}

ERef<EProperty> EBaseTask::Execute(EWeakRef<EProperty> inValue) 
{
    if (fHasInput)
    {
        if (inValue.expired() || inValue.lock() == nullptr)
        {
            E_ERROR("EBaseTask::Execute: input value is null");
            return nullptr;
        }
    }
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

void EBaseTask::SetInputDescription(const EValueDescription& description) 
{
    fInputDescription = description;
}

const EValueDescription& EBaseTask::GetInputDescription() const
{
    return fInputDescription;
}

void EBaseTask::SetOutputDescription(const EValueDescription& description) 
{
    fOutputDescription = description;
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

ERef<EProperty> ECFuncTask::OnExecute(EWeakRef<EProperty> inValue) 
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
    fExecuteFunction = [func](EWeakRef<EProperty> inValue) -> ERef<EProperty>
    {
        func();
        return nullptr;
    };
}

void ECFuncTask::SetFunc(CFunc_NoParam_Return func) 
{
    fHasInput = false;
    fHasOutput = true;
    fExecuteFunction = [func](EWeakRef<EProperty> inValue) -> ERef<EProperty>
    {
        return func();
    };
}

void ECFuncTask::SetFunc(CFunc_Param_NoReturn func) 
{
    fHasInput = true;
    fHasOutput = false;
    fExecuteFunction = [func](EWeakRef<EProperty> inValue) -> ERef<EProperty>
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