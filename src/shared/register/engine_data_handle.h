#pragma once

namespace Engine {

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

        STRUCTURE,
        STRUCTURE_REF
    };

    struct E_API EDataDescriptor
    {
        EString     DataName;
        EDataType   DataType;
    };

    class E_API EDataHandle
    {
    private:
        EDataDescriptor fDataDescription;
    protected:
        EDataHandle(const EString& name, EDataType type);
    public:
        virtual ~EDataHandle() = default;

        const EString& GetName() const;
        EDataType GetDataType() const;
    };

    class E_API EIntegerDataHandle : public EDataHandle
    {
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


    class E_API EStructureDataHandle : public EDataHandle
    {
        using FieldMap = EUnorderedMap<EString, ERef<EDataHandle>>;
    private:
        FieldMap    fFields;
    public:
        EStructureDataHandle(const EString& name);
        ~EStructureDataHandle();

        bool GetFieldAt(const EString& name);

        void AddField(EDataDescriptor descriptor);

        const FieldMap& GetFields();
        FieldMap::iterator begin();
        FieldMap::iterator end();
        FieldMap::const_iterator begin() const;
        FieldMap::const_iterator end() const;
    };

}