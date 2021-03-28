#pragma once

namespace Engine {

    template <typename T>
    struct EIsHandleClass
    {
        static constexpr bool value = false;
    };

    template <typename T>
    static constexpr bool EIsNotHandleClass = !(EIsHandleClass<T>::value);


    template <typename T>
    struct EPrimitiveTypeMap
    {
        using type = void;
    };

    /**
     * All Supported Data types in the engine
     */
    enum class EDataType
    {
        UNKNOWN = 0,

        INTEGER,
        FLOAT,
        BOOLEAN,
        STRING,
        //VECTOR2,
        //VECTOR3,
        //VECTOR4,

        //ARRAY,

        STRUCTURE,          // Some map-combination of types
        //DATA_REF       // Pointer to other data
    };

    /**
     * Hold the Name and Type of the data
     */
    struct E_API EDataDescriptor
    {
        EString     DataName;
        EDataType   DataType;
    };

    /**
     * The base class for all the Data Handles
     */
    class E_API EDataHandle
    {
    public:
        static EDataType data_type; // The static type. This is reuired in all sub-classes for static type checking
    private:
        EDataDescriptor fDataDescription;
    protected:
        EDataHandle(const EString& name, EDataType type);
    public:
        E_DEF_CCTOR(EDataHandle);
        virtual ~EDataHandle() = default;

        const EString& GetName() const;
        EDataType GetDataType() const;
    };

    template <>
    struct EIsHandleClass<EDataHandle>
    {
        static constexpr bool value = true;
    };

    /**
     * Integer Data field
     */
    class E_API EIntegerDataHandle : public EDataHandle
    {
    public:
        static EDataType data_type;
    private:
        i32     fValue;
    public:
        EIntegerDataHandle(const EString& name, i32 defaultValue = 0);
        E_DEF_CCTOR(EIntegerDataHandle);

        i32 GetValue() const;
        void SetValue(i32 value);


        operator i32() const;
        void operator=(i32 value);
    };

    template <>
    struct EIsHandleClass<EIntegerDataHandle>
    {
        static constexpr bool value = true;
    };

    template <>
    struct EPrimitiveTypeMap<int>
    {
        using type = EIntegerDataHandle;
    };



    /**
     * Float Data field
     */
    class E_API EFloatDataHandle : public EDataHandle
    {
    public:
        static EDataType data_type;
    private:
        float     fValue;
    public:
        EFloatDataHandle(const EString& name, float defaultValue = 0);
        E_DEF_CCTOR(EFloatDataHandle);

        float GetValue() const;
        void SetValue(float value);


        operator float() const;
        void operator=(float value);
    };

    template <>
    struct EIsHandleClass<EFloatDataHandle>
    {
        static constexpr bool value = true;
    };

    template <>
    struct EPrimitiveTypeMap<float>
    {
        using type = EFloatDataHandle;
    };
    template <>
    struct EPrimitiveTypeMap<double>
    {
        using type = EFloatDataHandle;
    };

    /**
     * Bool Data field
     */
    class E_API EBooleanDataHandle : public EDataHandle
    {
    public:
        static EDataType data_type;
    private:
        bool     fValue;
    public:
        EBooleanDataHandle(const EString& name, bool defaultValue = false);
        E_DEF_CCTOR(EBooleanDataHandle);

        bool GetValue() const;
        void SetValue(bool value);


        operator bool() const;
        void operator=(bool value);
    };


    template <>
    struct EIsHandleClass<EBooleanDataHandle>
    {
        static constexpr bool value = true;
    };

    template <>
    struct EPrimitiveTypeMap<bool>
    {
        using type = EBooleanDataHandle;
    };

    /**
     * String Data field
     */
    class E_API EStringDataHandle : public EDataHandle
    {
    public:
        static EDataType data_type;
    private:
        EString     fValue;
    public:
        EStringDataHandle(const EString& name, const EString& defaultValue = "");
        E_DEF_CCTOR(EStringDataHandle);

        const EString& GetValue() const;
        void SetValue(const EString& value);


        operator const EString&() const;
        void operator=(const EString& value);
    };

    template <>
    struct EIsHandleClass<EStringDataHandle>
    {
        static constexpr bool value = true;
    };

    template <>
    struct EPrimitiveTypeMap<EString>
    {
        using type = EStringDataHandle;
    };


    /**
     * Structure Data handle.
     * This Stores a map containing data handles. This is like a struct in c/c++
     */
    class E_API EStructureDataHandle : public EDataHandle
    {
        using FieldMap = EUnorderedMap<EString, ERef<EDataHandle>>;
    public:
        static EDataType data_type;
    private:
        FieldMap    fFields;
    public:
        EStructureDataHandle(const EString& name);
        E_DEF_CCTOR(EStructureDataHandle);
        ~EStructureDataHandle();

        /**
         * @param name The Field name to look in the map
         * @return Field at map entry. nullptr if not found
         */
        ERef<EDataHandle> GetFieldAt(const EString& name);
        /**
         * @return Wether the field is found
         */
        bool HasFieldAt(const EString& name);


        /**
         * @return Map of Fields
         */
        const FieldMap& GetFields();
        // Lopping
        FieldMap::iterator begin();
        FieldMap::iterator end();
        FieldMap::const_iterator begin() const;
        FieldMap::const_iterator end() const;

        // ---------------------------------------------
        // Templates

        /**
         * Adds an field to the structure
         * @param descriptor The minimum data to describe a DataHandle
         */
        template <typename Field, typename... Args>
        auto AddField(const EString& name, Args&& ... args)
        -> decltype(EIsHandleClass<Field>::value, void())
        {
            E_ASSERT(!HasFieldAt(name), "Field already exists!");
            if constexpr (!EIsHandleClass<Field>::value)
            {
                using FieldDataType = typename EPrimitiveTypeMap<Field>::type;
                if constexpr (!std::is_same<FieldDataType, void>())
                {
                    EDataType dataType = FieldDataType::data_type;
                    E_ASSERT(dataType != EDataType::UNKNOWN, "Cant add unknown data type to structure data called " + name);
                    fFields.insert({name, EMakeRef<FieldDataType>(name, args...)});
                }
            }
            else
            {
                EDataType dataType = Field::data_type;
                E_ASSERT(dataType != EDataType::UNKNOWN, "Cant add unknown data type to structure data called " + name);
                fFields.insert({name, EMakeRef<Field>(name, args...)});
            }
        }


        /**
         * @param name The Field name to look in the map
         * @return Field at map entry. nullptr if not found or type missmatched
         */
        template<typename T>
        ERef<T> GetFieldAt(const EString& name)
        {
            ERef<EDataHandle> result = GetFieldAt(name);
            if (!result) { return nullptr; }
            if (result->GetDataType() == T::data_type)
            {
                return std::dynamic_pointer_cast<T>(result);
            }
            E_WARN("Could not get field " + name + " from " + GetName() + "! WRONG TYPE");
            return nullptr;
        }
    };


    template <>
    struct EIsHandleClass<EStructureDataHandle>
    {
        static constexpr bool value = true;
    };


}