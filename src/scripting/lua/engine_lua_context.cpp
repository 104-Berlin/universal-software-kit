#include "prefix_scripting.h"

using namespace Engine;

static const char* REGISTRY_CONTEXT_KEY = "__current_contenxt";

static ELuaContext* instance = nullptr;

ELuaContext::ELuaContext()
    : EScriptContext("Lua-Context")
{
    if (instance != nullptr)
    {
        E_ERROR("ELuaContext::ELuaContext: instance already exists");
    }
    instance = this;
    fLuaState = luaL_newstate();
    luaL_openlibs(fLuaState);
}

ELuaContext::~ELuaContext()
{
    lua_close(fLuaState);
    if (instance == this)
    {
        instance = nullptr;
    }
    else
    {
        E_ERROR("ELuaContext::~ELuaContext: instance is not the current instance");
    }
}

void ELuaContext::RegisterTask(EBaseTask* task)
{
    EScriptContext::RegisterTask(task);
    const EValueDescription& inputDescription = task->GetInputDescription();
    const EValueDescription& outputDescription = task->GetOutputDescription();

    lua_CFunction fn = [](lua_State* L)->int{
        E_INFO(EString("Lua function called: ") + std::to_string(lua_gettop(L)));
        lua_Debug lua_info;
        if (lua_getinfo(L, "name", &lua_info) == 0)
        {
            E_INFO(EString("Calling ") + lua_info.name);
            instance->Run_Task(lua_info.name, L);
        }

        return 0;
    };
    lua_pushcfunction(fLuaState, fn);
    lua_setglobal(fLuaState, task->GetName().c_str()); 
}

void ELuaContext::Execute(const EString& command)
{
    if (luaL_dostring(fLuaState, command.c_str()) != 0)
    {
        E_ERROR("Error executing command: " + command);
        E_ERROR(EString("Error: ") + lua_tostring(fLuaState, -1));
        lua_pop(fLuaState, 1);
    }
}

void ELuaContext::Run_Task(const EString& taskName, lua_State* state)
{
    // Build the input value
    EBaseTask* taskToRun = GetTask(taskName);
    if (taskToRun == nullptr)
    {
        E_ERROR("ELuaContext::Run_Task: task not found: " + taskName);
        return;
    }
    // Get to input description
    const EValueDescription& inputDescription = taskToRun->GetInputDescription();
    if (inputDescription.GetType() == EValueType::STRUCT)
    {
        // Each struct element is one input of function call
        for (auto& field : inputDescription.GetStructFields())
        {
            
        }
    }
    ERef<EProperty> inputValue = CreatePropertyFromLua(inputDescription, "", state, 1);
    RunTask(taskName, inputValue);
}

ERef<EProperty> ELuaContext::CreatePropertyFromLua(const EValueDescription& description, const EString& name, lua_State* state, int index)
{
    ERef<EProperty> result = nullptr;
    switch (description.GetType())
    {
    case EValueType::STRUCT:
        result = CreatePropertyFromLua_Struct(description, name, state, index);
        break;
    case EValueType::PRIMITIVE:
        result = CreatePropertyFromLua_Primitive(description, name, state, index);
        break;
    case EValueType::ENUM:
        result = CreatePropertyFromLua_Enum(description, name, state, index);
        break;
    case EValueType::ARRAY:
        result = CreatePropertyFromLua_Array(description, name, state, index);
        break;
    default:
        E_ERROR("ELuaContext::CreatePropertyFromLua: unknown type!");
        break;
    }
    return result;
}

ERef<EProperty> ELuaContext::CreatePropertyFromLua_Struct(const EValueDescription& description, const EString& name, lua_State* state, int index)
{
    return nullptr;
}

ERef<EProperty> ELuaContext::CreatePropertyFromLua_Primitive(const EValueDescription& description, const EString& name, lua_State* state, int index)
{
    const EString& primitiveId = description.GetId();

#define CREATE_PRIMITIVE(type, value) ERef<EValueProperty<type>> result = EMakeRef<EValueProperty<type>>(name, description, (value)); \
                                            return result;

    if (primitiveId == E_TYPEID_STRING) { CREATE_PRIMITIVE(EString, lua_tostring(state, index)) } 
    else if (primitiveId == E_TYPEID_INTEGER) { CREATE_PRIMITIVE(i32, lua_tonumber(state, index)) }
    else if (primitiveId == E_TYPEID_UNSIGNED_INTEGER) {  }
    else if (primitiveId == E_TYPEID_UNSIGNED_BIG_INTEGER) {  }
    else if (primitiveId == E_TYPEID_DOUBLE) {  }
    else if (primitiveId == E_TYPEID_FLOAT) {  }
    else if (primitiveId == E_TYPEID_BOOL) {  }

    return nullptr;
}

ERef<EProperty> ELuaContext::CreatePropertyFromLua_Enum(const EValueDescription& description, const EString& name, lua_State* state, int index)
{
    return nullptr;
}

ERef<EProperty> ELuaContext::CreatePropertyFromLua_Array(const EValueDescription& description, const EString& name, lua_State* state, int index)
{
    return nullptr;
}

