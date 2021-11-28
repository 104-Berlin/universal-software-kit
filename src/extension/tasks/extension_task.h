#pragma once

namespace Engine {

    static constexpr u16 ETaskType_TASK_NONE = 0;
    static constexpr u16 ETaskType_SINGLE_EXECUTION = BIT(1);
    static constexpr u16 ETaskType_ALLWAYS_RUNNING = BIT(2);
    static constexpr u16 ETaskType_EVENT_DRIVEN = BIT(3);


    class EBaseTask
    {
    private:
        EString fName;

        u16 fType;
    public:
        EBaseTask(const EString& name, u16 type);
        ~EBaseTask();

        EStructProperty* Execute(EStructProperty* inValue);
        virtual EStructProperty* OnExecute(EStructProperty* inValue) = 0;
    };

    class ECFuncTask : public EBaseTask
    {
    private:
        using CFunc_NoParam_NoReturn = std::function<void()>;
        using CFunc_NoParam_Return = std::function<EStructProperty*()>;
        using CFunc_Param_NoReturn = std::function<void(EStructProperty*)>;
        using CFunc_Param_Return = std::function<EStructProperty*(EStructProperty*)>;

        CFunc_Param_Return fExecuteFunction;
    public:
        ECFuncTask(const EString& name);
        ECFuncTask(const EString& name, CFunc_Param_Return func);
        ECFuncTask(const EString& name, CFunc_Param_NoReturn func);
        ECFuncTask(const EString& name, CFunc_NoParam_Return func);
        ECFuncTask(const EString& name, CFunc_NoParam_NoReturn func);
        ~ECFuncTask();

        EStructProperty* OnExecute(EStructProperty* inValue);

        void SetFunc(CFunc_NoParam_NoReturn func);
        void SetFunc(CFunc_NoParam_Return func);
        void SetFunc(CFunc_Param_NoReturn func);
        void SetFunc(CFunc_Param_Return func);
    };

}