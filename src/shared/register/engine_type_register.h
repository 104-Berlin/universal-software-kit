#pragma once

namespace Engine {

    enum class EValueType
    {
        UNKNOWN,

        STRUCT,
        ENUM,
        PRIMITIVE
    };

    class E_API EValueDescription
    {
    public:
        using t_ID = EString;
    private:
        EValueType  fType;
        t_ID        fID;

        // For struct
        EUnorderedMap<EString, struct EValueDescription> fStructFields;
        // For enum
        EVector<EString> fEnumOptions;
        // For array
        bool fIsArray;
    public:
        EValueDescription(EValueType type = EValueType::UNKNOWN, t_ID id = "");
        EValueDescription(const EValueDescription&) = default;
        ~EValueDescription();

        EValueType GetType() const;
        const t_ID& GetId() const;

        bool Valid() const;

        // For structs
        EValueDescription& AddStructField(const EString& name, EValueDescription description);
        const EUnorderedMap<EString, EValueDescription>& GetStructFields() const;

        // For enums
        EValueDescription& AddEnumOption(const EString& option);
        const EVector<EString>& GetEnumOptions() const;

        // Array
        bool IsArray() const;
        EValueDescription GetAsArray() const;
        EValueDescription GetAsPrimitive() const;
    };


    #define E_TYPEID_STRING "string"
    #define E_TYPEID_INTEGER "int"
    #define E_TYPEID_DOUBLE "double"
    #define E_TYPEID_BOOL "bool"


    
    static EValueDescription StringDescription(EValueType::PRIMITIVE, E_TYPEID_STRING);
    static EValueDescription IntegerDescription(EValueType::PRIMITIVE, E_TYPEID_INTEGER);
    static EValueDescription DoubleDescription(EValueType::PRIMITIVE, E_TYPEID_DOUBLE);
    static EValueDescription BoolDescription(EValueType::PRIMITIVE, E_TYPEID_BOOL);
}