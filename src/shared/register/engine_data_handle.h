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
        EDataDescriptor(const EString& name, EDataType type) : DataName(name), DataType(type) {}
        EString     DataName;
        EDataType   DataType;
    };

    /**
     * Description of data.
     */
    class E_API EStructureDescription
    {
    private:
        EDataDescriptor                 fTypeData;
        EVector<EStructureDescription>  fChilds;
    public:
        EStructureDescription(EDataDescriptor typeData, const EVector<EStructureDescription>&  childs = {});
        virtual ~EStructureDescription() = default;

        EDataDescriptor GetTypeData() const;

        const EVector<EStructureDescription>& GetChilds() const;
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

        float GetValue() const;
        void SetValue(float value);


        operator float() const;
        void operator=(float value);
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
        EStructureDataHandle(const EString& name, const EStructureDescription& description);
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
    private:
        void AddFieldsFromDescpription(const EStructureDescription& description);
    };

}

#define E_BEGIN_STRUCT(name) Engine::EStructureDescription name; \
                            [&name](){\
                             Engine::EStructureDescription& currently_setting = name;

#define E_STRUCT_DATA(name, type) currently_setting.Childs.push_back({{#name, type}});

#define E_END_STRUCT() }()