#pragma once

#include "lua.hpp"

namespace Engine {


    class E_SAPI ELuaContext : public EScriptContext
    {
    private:
        lua_State* fLuaState;
    public:
        ELuaContext();
        ~ELuaContext();

        virtual void RegisterTask(EBaseTask* task) override;
        virtual void Execute(const EString& command) override;
    private:
        void Run_Task(const EString& taskName, lua_State* state);

        ERef<EProperty> CreatePropertyFromLua(const EValueDescription& description, const EString& name, lua_State* state, int index);
        ERef<EProperty> CreatePropertyFromLua_Struct(const EValueDescription& description, const EString& name, lua_State* state, int index);
        ERef<EProperty> CreatePropertyFromLua_Primitive(const EValueDescription& description, const EString& name, lua_State* state, int index);
        ERef<EProperty> CreatePropertyFromLua_Enum(const EValueDescription& description, const EString& name, lua_State* state, int index);
        ERef<EProperty> CreatePropertyFromLua_Array(const EValueDescription& description, const EString& name, lua_State* state, int index);
        ERef<EProperty> CreatePropertyFromLua_Any(const EValueDescription& description, const EString& name, lua_State* state, int index);
    };

}