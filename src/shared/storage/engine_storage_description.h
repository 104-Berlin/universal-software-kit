#pragma once

namespace Engine {

    enum class EValueType
    {
        UNKNOWN,

        STRUCT,
        ARRAY,
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
        EValueDescription* fArrayType;
        
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
        void SetArrayType(const EValueDescription& type);

        EValueDescription GetAsArray() const;
        EValueDescription GetAsPrimitive() const;



        static EValueDescription CreateStruct(const t_ID& id, std::initializer_list<std::pair<EString, EValueDescription>> childs);

        bool operator==(const EValueDescription& other);
        bool operator!=(const EValueDescription& other);
    };


    #define E_TYPEID_STRING "string"
    #define E_TYPEID_INTEGER "int"
    #define E_TYPEID_UNSIGNED_INTEGER "uint"
    #define E_TYPEID_UNSIGNED_BIG_INTEGER "ulonglong"
    #define E_TYPEID_DOUBLE "double"
    #define E_TYPEID_BOOL "bool"

    #define StringDescription ::Engine::EValueDescription(::Engine::EValueType::PRIMITIVE, E_TYPEID_STRING)
    #define IntegerDescription ::Engine::EValueDescription(::Engine::EValueType::PRIMITIVE, E_TYPEID_INTEGER)
    #define UnsignedIntegerDescription ::Engine::EValueDescription(::Engine::EValueType::PRIMITIVE, E_TYPEID_UNSIGNED_INTEGER)
    #define UnsignedBigIntegerDescription ::Engine::EValueDescription(::Engine::EValueType::PRIMITIVE, E_TYPEID_UNSIGNED_BIG_INTEGER)
    #define DoubleDescription ::Engine::EValueDescription(::Engine::EValueType::PRIMITIVE, E_TYPEID_DOUBLE)
    #define BoolDescription ::Engine::EValueDescription(::Engine::EValueType::PRIMITIVE, E_TYPEID_BOOL)


    
    namespace getdsc {

        template <typename T>
        EValueDescription GetDescription()
        {
            if constexpr (is_vector<T>::value)
            {
                return GetDescription<typename T::value_type>().GetAsArray();
            }
            else if constexpr (std::is_same<T, EString>())
            {
                return StringDescription;
            }
            else if constexpr (std::is_same<T, double>())
            {
                return DoubleDescription;
            }
            else if constexpr (std::is_same<T, float>())
            {
                return DoubleDescription;
            }
            else if constexpr (std::is_same<T, bool>())
            {
                return BoolDescription;
            }
            else if constexpr (is_one_of<T, int>())
            {
                return IntegerDescription;
            }
            else if constexpr (is_one_of<T, u32, unsigned int>())
            {
                return UnsignedIntegerDescription;
            }
            else if constexpr (is_one_of<T, u64, unsigned long long>())
            {
                return UnsignedBigIntegerDescription;
            }
            else 
            {
                return T::_dsc;
            }
            E_ASSERT_M(false, EString("Could not find description of type ") + typeid(T).name());
            return EValueDescription();
        }

    }
}