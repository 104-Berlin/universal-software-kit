#pragma once

namespace Engine {

    class EScriptingManager 
    {
    private:
        EVector<EScriptContext*> fLoadedContexts;
    public:
        EScriptingManager();
        ~EScriptingManager();

        template <typename T>
        T* LoadScriptContext()
        {
            T* result = new T();
            fLoadedContexts.push_back(result);
            return result;
        }

        template <typename T>
        T* GetScriptContext()
        {
            for (auto& context : fLoadedContexts)
            {
                if (typeid(*context) == typeid(T))
                {
                    return static_cast<T*>(context);
                }
            }
            return nullptr;
        }

        void InitDefaultContexts(const EVector<EBaseTask*>& tasks);
    };

}