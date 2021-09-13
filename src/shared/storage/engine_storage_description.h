#pragma once

namespace Engine {

    enum class EValueType
    {
        UNKNOWN,

        STRUCT,
        ENUM,
        PRIMITIVE
    };

    /**
     * @brief Description of a value. Used for dynamically reflect values.
     */
    class E_API EValueDescription
    {
    public:
        using t_ID = EString;
    private:
        /**
         * @brief The type of the description. Ether the description is a Primitive, a struct or an enum
         */
        EValueType  fType;
        /**
         * @brief ID of the type. 
         * For primitives we use the at the bottom descripted ID's
         * For structs and enums it is the typename
         */
        t_ID        fID;

        // For enum
        /**
         * @brief All options for enums
         */
        EVector<EString> fEnumOptions; 
        // For array
        /**
         * @brief Is the type an array
         */
        bool fIsArray;

        // For struct
        /**
         * @brief All Fields for struct
         */
        EUnorderedMap<EString, EValueDescription*> fStructFields;
    public:
        EValueDescription(EValueType type = EValueType::UNKNOWN, t_ID id = "");
        EValueDescription(const EValueDescription&);
        EValueDescription& operator=(const EValueDescription& other);
        ~EValueDescription();

        /**
         * @return The ValueType
         */
        EValueType GetType() const;
        /**
         * @return The ID
         */
        const t_ID& GetId() const;

        /**
         * @return Wether the type is valid. The type is invalid if the type is UNKNOWN or the id is empty
         */
        bool Valid() const;

        // For structs
        /**
         * @brief Add a field description to the struct descpriptions
         * @param name The name of the Field
         * @param description The description to insert
         * @return Reference to this pointer
         */
        EValueDescription& AddStructField(const EString& name, EValueDescription description);
        /**
         * @brief Get all the Fields of the struct description
         * @return All struct Fields
         */
        const EUnorderedMap<EString, EValueDescription*>& GetStructFields() const;

        // For enums
        /**
         * @brief Add an enum option to description
         * @return Reference to this pointer
         */
        EValueDescription& AddEnumOption(const EString& option);

        /**
         * @brief Gets all enum options
         * @return Enum options
         */
        const EVector<EString>& GetEnumOptions() const;

        // Array
        /**
         * @brief Wether the dscription is an array
         * @return Is array
         */
        bool IsArray() const;

        /**
         * @brief Make the description an array with the same type
         * @return Copy of this description with fIsArray = true
         */
        EValueDescription GetAsArray() const;

        /**
         * @brief Make the description a single value, not an array
         * @return Copy of this description with fIsArray = false
         */
        EValueDescription GetAsPrimitive() const;


        /**
         * @brief Creates a new Struct Description. Used to easely create static Descriptions
         * @param id Name/ID of the struct
         * @param child Pair-List of (EString fieldName, EValueDescription dsc)
         * @return New Struct-Description
         */
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
        
        /**
         * @brief Gets the description by template. You can specilize the function for custom types. Use the E_STORAGE_STRUCT macro and you dont need to worry specializing this
         */
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