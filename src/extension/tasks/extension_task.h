#pragma once

namespace Engine {

    static constexpr u16 ETaskType_TASK_NONE = 0;
    static constexpr u16 ETaskType_SINGLE_EXECUTION = BIT(1);
    static constexpr u16 ETaskType_ALLWAYS_RUNNING = BIT(2);
    static constexpr u16 ETaskType_EVENT_DRIVEN = BIT(3);


    class E_EXTAPI EBaseTask
    {
    private:
        EString fName;

        u16 fType;
    protected:
        bool fHasInput;
        bool fHasOutput;

        EValueDescription fInputDescription;
        EValueDescription fOutputDescription;
    public:
        EBaseTask(const EString& name, u16 type, bool hasInput = false, bool hasOutput = false);
        virtual ~EBaseTask();

        ERef<EProperty> Execute(EWeakRef<EProperty> inValue);
        virtual ERef<EProperty> OnExecute(EWeakRef<EProperty> inValue) = 0;

        const EString& GetName() const;

        bool HasInput() const;
        bool HasOutput() const;

        void SetInputDescription(const EValueDescription& description);
        const EValueDescription& GetInputDescription() const;
        void SetOutputDescription(const EValueDescription& description);
        const EValueDescription& GetOutputDescription() const;
    };

    class E_EXTAPI ECFuncTask : public EBaseTask
    {
    private:
        using CFunc_NoParam_NoReturn = std::function<void()>;
        using CFunc_NoParam_Return = std::function<ERef<EProperty>()>;
        using CFunc_Param_NoReturn = std::function<void(EWeakRef<EProperty>)>;
        using CFunc_Param_Return = std::function<ERef<EProperty>(EWeakRef<EProperty>)>;

        CFunc_Param_Return fExecuteFunction;
    public:
        ECFuncTask(const EString& name);
        ECFuncTask(const EString& name, CFunc_Param_Return func);
        ECFuncTask(const EString& name, CFunc_Param_NoReturn func);
        ECFuncTask(const EString& name, CFunc_NoParam_Return func);
        ECFuncTask(const EString& name, CFunc_NoParam_NoReturn func);
        ~ECFuncTask();

        ERef<EProperty> OnExecute(EWeakRef<EProperty> inValue);

        void SetFunc(CFunc_NoParam_NoReturn func);
        void SetFunc(CFunc_NoParam_Return func);
        void SetFunc(CFunc_Param_NoReturn func);
        void SetFunc(CFunc_Param_Return func);
    };

}