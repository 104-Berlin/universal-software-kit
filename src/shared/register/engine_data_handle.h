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
        static EDataType data_type; // The static type. This is reuired in all sub-classes
    private:
        EDataDescriptor fDataDescription;
    protected:
        EDataHandle(const EString& name, EDataType type);
    public:
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
        ~EIntegerDataHandle();

        i32 GetValue() const;
        void SetValue(i32 value);


        operator i32() const;
        void operator=(i32 value);
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
         * Adds an field to the structure
         * @param descriptor The minimum data to describe a DataHandle
         */
        void AddField(EDataDescriptor descriptor);

        /**
         * @return Map of Fields
         */
        const FieldMap& GetFields();
        // Lopping
        FieldMap::iterator begin();
        FieldMap::iterator end();
        FieldMap::const_iterator begin() const;
        FieldMap::const_iterator end() const;


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