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
        EUnorderedMap<EString, ERef<EValueDescription>> fFields;
    public:
        EStructDescription(const EString& name);
        virtual ~EStructDescription();

        EStructDescription& AddField(const EString& name, ERef<EValueDescription> description);

        const EUnorderedMap<EString, ERef< EValueDescription>>& GetFields() const;
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
        ERef<EValueDescription> fType;
    public:
        EArrayDescription(ERef<EValueDescription> arrayType);
        virtual ~EArrayDescription();

        ERef<EValueDescription> GetElementType() const;
    };


    #define E_TYPEID_STRING "string"
    #define E_TYPEID_INTEGER "int"
    #define E_TYPEID_DOUBLE "double"
    #define E_TYPEID_BOOL "bool"


    
    E_API ERef<EValueDescription> StringDescription();
    E_API ERef<EValueDescription> IntegerDescription();
    E_API ERef<EValueDescription> DoubleDescription();
    E_API ERef<EValueDescription> BoolDescription();
}