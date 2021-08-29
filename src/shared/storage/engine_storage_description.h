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

        // For enum
        EVector<EString> fEnumOptions;
        // For array
        bool fIsArray;
        // For struct
        EUnorderedMap<EString, EValueDescription*> fStructFields;
    public:
        EValueDescription(EValueType type = EValueType::UNKNOWN, t_ID id = "");
        EValueDescription(const EValueDescription&);
        EValueDescription& operator=(const EValueDescription& other);
        ~EValueDescription();

        EValueType GetType() const;
        const t_ID& GetId() const;

        bool Valid() const;

        // For structs
        EValueDescription& AddStructField(const EString& name, EValueDescription description);
        const EUnorderedMap<EString, EValueDescription*>& GetStructFields() const;

        // For enums
        EValueDescription& AddEnumOption(const EString& option);
        const EVector<EString>& GetEnumOptions() const;

        // Array
        bool IsArray() const;
        EValueDescription GetAsArray() const;
        EValueDescription GetAsPrimitive() const;



        static EValueDescription CreateStruct(const t_ID& id, std::initializer_list<std::pair<EString, EValueDescription>> childs);

        bool operator==(const EValueDescription& other);
        bool operator!=(const EValueDescription& other);
    };


    #define E_TYPEID_STRING "string"
    #define E_TYPEID_INTEGER "int"
    #define E_TYPEID_DOUBLE "double"
    #define E_TYPEID_BOOL "bool"


    
    static EValueDescription StringDescription(EValueType::PRIMITIVE, E_TYPEID_STRING);
    static EValueDescription IntegerDescription(EValueType::PRIMITIVE, E_TYPEID_INTEGER);
    static EValueDescription DoubleDescription(EValueType::PRIMITIVE, E_TYPEID_DOUBLE);
    static EValueDescription BoolDescription(EValueType::PRIMITIVE, E_TYPEID_BOOL);


    
    namespace getdsc {

        template<typename T> struct is_vector : public std::false_type {};

    template<typename T, typename A>
    struct is_vector<std::vector<T, A>> : public std::true_type {};

        template <typename T>
        EValueDescription GetDescription()
        {
            if constexpr (is_vector<T>::value)
            {
                return GetDescription<T::value_type>().GetAsArray();
            }
            else
            {
                return T::_dsc;
            }
        }


        extern template E_API EValueDescription getdsc::GetDescription<EString>();
        extern template E_API EValueDescription getdsc::GetDescription<int>();
        extern template E_API EValueDescription getdsc::GetDescription<float>();
        extern template E_API EValueDescription getdsc::GetDescription<double>();
        extern template E_API EValueDescription getdsc::GetDescription<bool>();
    }
}