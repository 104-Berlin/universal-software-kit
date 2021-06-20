#include "prefix_scripting.h"

using namespace Engine;

EChaiContext::EChaiContext() 
    : EScriptContext("Chai Context")
{
    
}

void EChaiContext::Execute(const EString& command) 
{
    try
    {
        fContext.eval(command);
    }
    catch (chaiscript::exception::eval_error err)
    {
        E_ERROR("ERROR: Evaluating Chai script!\n" + err.reason);
    }
}