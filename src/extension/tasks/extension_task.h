#pragma once

namespace Engine {

    E_STORAGE_ENUM(ETaskType,
        SINGLE_EXECUTION,
        ALLWAYS_RUNNING,
        EVENT_DRIVEN
    );


    class EBaseTask
    {
    private:
        EString fName;

        ETaskType fType;
    public:
        EBaseTask(const EString& name, ETaskType type);
        ~EBaseTask();

        EStructProperty* Execute(EStructProperty* inValue);
        virtual EStructProperty* OnExecute(EStructProperty* inValue) = 0;
    };

}