#include "prefix_scripting.h"

using namespace Engine;

EChaiContext::EChaiContext() 
    : EScriptContext("Chai Context")
{

}

void EChaiContext::Execute(const EString& command) 
{
    fContext.eval(command);
}