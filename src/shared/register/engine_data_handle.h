#pragma once

namespace Engine {

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
        VECTOR2,
        VECTOR3,
        VECTOR4,

        ARRAY,

        STRUCTURE,          // Some map-combination of types
        DATA_REF       // Pointer to other data
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
        ~EIntegerDataHandle();

        i32 GetValue() const;
        void SetValue(i32 value);


        operator i32() const;
        void operator=(i32 value);
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
        ~EFloatDataHandle();

        float GetValue() const;
        void SetValue(float value);


        operator float() const;
        void operator=(float value);
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
        template <typename... Args>
        void AddField(EDataDescriptor descriptor, Args&& ... args)
        {
            switch (descriptor.DataType)
            {
            case Engine::EDataType::UNKNOWN:
                E_WARN("Cant add unknown data type to StructureData called " + descriptor.DataName);
                break;
            case Engine::EDataType::INTEGER:
                fFields.insert({descriptor.DataName, EMakeRef<EIntegerDataHandle>(descriptor.DataName, args...)});
                break;
            case Engine::EDataType::FLOAT:
                break;
            case Engine::EDataType::BOOLEAN:
                break;
            case Engine::EDataType::STRING:
                break;
            case Engine::EDataType::VECTOR2:
                break;
            case Engine::EDataType::VECTOR3:
                break;
            case Engine::EDataType::VECTOR4:
                break;
            case Engine::EDataType::ARRAY:
                break;
            case Engine::EDataType::STRUCTURE:
                break;
            case Engine::EDataType::DATA_REF:
                break;
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

}