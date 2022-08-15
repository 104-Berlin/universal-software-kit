#include "prefix_scripting.h"

using namespace Engine;

EScriptingManager::EScriptingManager()
{
    
}

EScriptingManager::~EScriptingManager()
{
    
}

void EScriptingManager::InitDefaultContexts(const EVector<EBaseTask*>& tasks)
{
    ELuaContext* luaContext = LoadScriptContext<ELuaContext>();
    for (auto& task : tasks)
    {
        E_INFO("Loading task to lua scripting: " + task->GetName());
        luaContext->RegisterTask(task);
    }
}