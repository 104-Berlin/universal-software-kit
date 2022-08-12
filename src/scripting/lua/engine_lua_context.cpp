#include "prefix_scripting.h"

using namespace Engine;

namespace LuaHelper
{
    template <typename T>
    T GetLuaValue(lua_State* L, int index)
    {
        E_ERROR("Get field not implemented for given type");
        return T();
    }

    template <>
    i32 GetLuaValue(lua_State* L, int index)
    {
        if (!lua_isnumber(L, index))
        {
            E_ERROR("Error: field is not a number");
            return 0;
        }
        return (i32)lua_tonumber(L, index);
    }

    template <>
    u32 GetLuaValue(lua_State* L, int index)
    {
        if (!lua_isnumber(L, index))
        {
            E_ERROR("Error: field is not a number");
            return 0;
        }
        return (u32)lua_tonumber(L, index);
    }

    template <>
    u64 GetLuaValue(lua_State* L, int index)
    {
        if (!lua_isnumber(L, index))
        {
            E_ERROR("Error: field is not a number");
            return 0;
        }
        return (u64)lua_tonumber(L, index);
    }

    template <>
    float GetLuaValue(lua_State* L, int index)
    {
        if (!lua_isnumber(L, index))
        {
            E_ERROR("Error: field is not a number");
            return 0;
        }
        return (float)lua_tonumber(L, index);
    }

    template <>
    double GetLuaValue(lua_State* L, int index)
    {
        if (!lua_isnumber(L, index))
        {
            E_ERROR("Error: field is not a number");
            return 0;
        }
        return (double)lua_tonumber(L, index);
    }

    template <>
    EString GetLuaValue(lua_State* L, int index)
    {
        if (!lua_isstring(L, index))
        {
            E_ERROR("Error: field is not a string");
            return EString();
        }
        return EString(lua_tostring(L, index));
    }

    template <>
    bool GetLuaValue(lua_State* L, int index)
    {
        if (!lua_isboolean(L, index))
        {
            E_ERROR("Error: field is not a boolean");
            return false;
        }
        return lua_toboolean(L, index);
    }


    
    void PushTableFieldToStack(lua_State* L, const char* key, int tableIndex)
    {
        lua_pushstring(L, key);
        lua_gettable(L, tableIndex < 0 ? tableIndex - 1 : tableIndex);
    }

    /**
     * @brief Get the Table Field. 
     * 
     * @tparam T 
     * @param key - Name of the field
     * @return T 
     */
    template <typename T>
    T GetTableField(lua_State* L, const char* key, int tableIndex)
    {
        PushTableFieldToStack(L, key, tableIndex);
        T result = GetLuaValue<T>(-1);
        lua_pop(L, 1);
        return result;
    }

    void StackDump (lua_State *L) {
        printf("LuaStackDump\n");
        printf("------------\n");
        int i;
        int top = lua_gettop(L);
        for (i = 1; i <= top; i++) {  /* repeat for each level */
            int t = lua_type(L, i);
            switch (t) {
        
            case LUA_TSTRING:  /* strings */
                printf("`%s'", lua_tostring(L, i));
                break;
        
            case LUA_TBOOLEAN:  /* booleans */
                printf(lua_toboolean(L, i) ? "true" : "false");
                break;
        
            case LUA_TNUMBER:  /* numbers */
                printf("%g", lua_tonumber(L, i));
                break;
        
            default:  /* other values */
                printf("%s", lua_typename(L, t));
                break;
        
            }
            printf("\n");  /* put a separator */
        }
        printf("------------\n");
    }

    EVector<EString> GetLuaTableFields(lua_State* L, int index)
    {
        if (index < 0)
        {
            index = lua_gettop(L) + index + 1;
        }
        E_ASSERT(lua_istable(L, index));
        EVector<EString> fields;
        lua_pushnil(L);
        while (lua_next(L, index) != 0)
        {
            StackDump(L);
            fields.push_back(GetLuaValue<EString>(L, -2));
            lua_pop(L, 1);
        }
        return fields;
    }

    EValueDescription GetLuaTypeToValueDescription(int luaType)
    {
        switch (luaType)
        {
        case LUA_TNUMBER:
            return DoubleDescription;
        case LUA_TBOOLEAN:
            return BoolDescription;
        case LUA_TSTRING:
            return StringDescription;
        case LUA_TTABLE:
            return EValueDescription(EValueType::STRUCT, "LuaTable");
        }
        return EValueType::UNKNOWN;
    }


    EValueDescription CreateDescriptionFromLuaValue(lua_State* L, int index)
    {
        int type = lua_type(L, index);
        EValueDescription result = GetLuaTypeToValueDescription(type);
        if (lua_istable(L, index))
        {
            EVector<EString> fields = GetLuaTableFields(L, index);

            for (const EString& field : fields)
            {
                PushTableFieldToStack(L, field.c_str(), index);
                result.AddStructField(field, CreateDescriptionFromLuaValue(L, -1));
                lua_pop(L, 1);
            }
        }
        return result;
    }
}




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
        lua_Debug lua_info;
        lua_getstack(L, 0, &lua_info);
        lua_getinfo(L, "n", &lua_info);
        E_INFO(EString("Calling ") + lua_info.name);
        instance->Run_Task(lua_info.name, L);
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
    /*if (inputDescription.GetType() == EValueType::STRUCT)
    {
        // Each struct element is one input of function call
        for (auto& field : inputDescription.GetStructFields())
        {
            
        }
    }*/

    //LuaHelper::StackDump(state);

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
    case EValueType::ANY:
        result = CreatePropertyFromLua_Any(description, name, state, index);
        break;
    default:
        E_ERROR("ELuaContext::CreatePropertyFromLua: unknown type!");
        break;
    }
    return result;
}

ERef<EProperty> ELuaContext::CreatePropertyFromLua_Struct(const EValueDescription& description, const EString& name, lua_State* state, int index)
{
    E_ASSERT(description.GetType() == EValueType::STRUCT);
    E_ASSERT(lua_istable(state, index));

    ERef<EStructProperty> result = std::dynamic_pointer_cast<EStructProperty>(EProperty::CreateFromDescription(name, description));
    for (const EValueDescription::StructField& field : description.GetStructFields())
    {
        LuaHelper::PushTableFieldToStack(state, field.first.c_str(), index);
        ERef<EProperty> fieldProperty = CreatePropertyFromLua(field.second, field.first, state, -1);
        if (fieldProperty && result->HasProperty(field.first))
        {
            result->GetProperty(field.first)->Copy(fieldProperty.get());
        }
        lua_pop(state, 1);
    }

    return result;
}

ERef<EProperty> ELuaContext::CreatePropertyFromLua_Primitive(const EValueDescription& description, const EString& name, lua_State* state, int index)
{
    const EString& primitiveId = description.GetId();

#define CREATE_PRIMITIVE(type, value) ERef<EValueProperty<type>> result = EMakeRef<EValueProperty<type>>(name, description, (value)); \
                                            return result;

    if (primitiveId == E_TYPEID_STRING) { CREATE_PRIMITIVE(EString, lua_tostring(state, index)) } 
    else if (primitiveId == E_TYPEID_INTEGER) { CREATE_PRIMITIVE(i32, (i32)lua_tonumber(state, index)) }
    else if (primitiveId == E_TYPEID_UNSIGNED_INTEGER) { CREATE_PRIMITIVE(u32, (u32) lua_tonumber(state, index)) }
    else if (primitiveId == E_TYPEID_UNSIGNED_BIG_INTEGER) { CREATE_PRIMITIVE(u64, (u64) lua_tonumber(state, index)) }
    else if (primitiveId == E_TYPEID_DOUBLE) { CREATE_PRIMITIVE(double, (double) lua_tonumber(state, index)) }
    else if (primitiveId == E_TYPEID_FLOAT) { CREATE_PRIMITIVE(float, (float) lua_tonumber(state, index)) }
    else if (primitiveId == E_TYPEID_BOOL) { CREATE_PRIMITIVE(bool, (bool) lua_toboolean(state, index)) }

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

ERef<EProperty> ELuaContext::CreatePropertyFromLua_Any(const EValueDescription& description, const EString& name, lua_State* state, int index)
{
    EValueDescription typeDsc = LuaHelper::CreateDescriptionFromLuaValue(state, index);
    if (!typeDsc.Valid()) { return nullptr; }

    ERef<EProperty> property = CreatePropertyFromLua(typeDsc, name, state, index);
    EAny resultAny;
    resultAny.SetValue(property);
    
    ERef<EProperty> result = EProperty::CreateFromTemplate<EAny>(name);
    result->SetValue(resultAny);
    return result;
}

