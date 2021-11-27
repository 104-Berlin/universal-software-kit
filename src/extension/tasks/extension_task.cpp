#include "engine_extension.h"

using namespace Engine;

EBaseTask::EBaseTask(const EString& name, ETaskType type) 
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