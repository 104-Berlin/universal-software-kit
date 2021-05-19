#pragma once

namespace Engine {

    enum class EValueType
    {
        STRUCT,
        ENUM,
        PRIMITIVE,
        ARRAY
    };

    class E_API EValueDescription
    {
        EValueType fType;
        EString    fID;
    public:
        EValueDescription(EValueType type, EString id);
        virtual ~EValueDescription();

        EValueType GetType() const;
        const EString& GetId() const;
        
    };

    class E_API EStructDescription : public EValueDescription
    {
        EUnorderedMap<EString, EValueDescription*> fFields;
    public:
        EStructDescription(const EString& name);
        virtual ~EStructDescription();

        EStructDescription& AddField(const EString& name, EValueDescription* description);

        const EUnorderedMap<EString, EValueDescription*>& GetFields() const;
    };

    class E_API EEnumDescription : public EValueDescription
    {
        EVector<EString> fOptions;
    public:
        EEnumDescription(const EString& name);
        virtual ~EEnumDescription();

        EEnumDescription& AddOption(const EString& option);
        const EVector<EString>& GetOptions() const;
    };

    class E_API EArrayDescription : public EValueDescription
    {
        EValueDescription* fType;
    public:
        EArrayDescription(EValueDescription* arrayType);
        virtual ~EArrayDescription();

        EValueDescription* GetElementType() const;
    };


    #define E_TYPEID_STRING "string"
    #define E_TYPEID_INTEGER "int"
    #define E_TYPEID_DOUBLE "double"
    #define E_TYPEID_BOOL "bool"


    
    E_API EValueDescription* StringDescription();
    E_API EValueDescription* IntegerDescription();
    E_API EValueDescription* DoubleDescription();
    E_API EValueDescription* BoolDescription();


    // Singleton for all registered types
    class E_API ETypeRegister
    {
    private:
        ETypeRegister();
        ETypeRegister(const ETypeRegister&) = default;
        ETypeRegister& operator=(const ETypeRegister&) = default;
    public:
        ~ETypeRegister();

        static ETypeRegister& get()
        {
            static ETypeRegister instance;
            return instance;
        }

        void RegisterDescription(EValueDescription* description);
        EValueDescription* FindById(const EString& id);

        EVector<EValueDescription*> GetAllDescriptions();
    private:
        EUnorderedMap<EString, EValueDescription*> fRegisteredDescriptions;
    };

}