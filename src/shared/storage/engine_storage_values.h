#pragma once

namespace Engine {

    class E_API EProperty
    {
        friend class ERegister;
    public:
        EString fName;
        EValueDescription fDescription;
    public:
        EProperty(const EString& name, EValueDescription description);
        EProperty(EProperty&) = default;
        virtual ~EProperty() = default;

        const EString& GetPropertyName() const;

        EValueDescription GetDescription() const;

        EProperty* Clone();
        
    protected:
        virtual EProperty* OnClone() = 0;

    public:
        static EProperty* CreateFromDescription(const EString& name, EValueDescription description);
    private:
        static EProperty* CreatePropertyStruct(const EString& name, EValueDescription description);
        static EProperty* CreatePropertyPrimitive(const EString& name, EValueDescription descrption);
        static EProperty* CreatePropertyEnum(const EString& name, EValueDescription descrption);
        static EProperty* CreatePropertyArray(const EString& name, EValueDescription description);
    };

    template <typename ValueType>
    class EValueProperty : public EProperty
    {
    private:
        ValueType fValue;
    public:
        EValueProperty(const EString& name, EValueDescription description, const ValueType& initValue = ValueType())
            : EProperty(name, description)
        {
            fValue = initValue;
        }

        EValueProperty(EValueProperty&) = default;

        void SetValue(const ValueType& value)
        {
            if (value == fValue) { return; }
            fValue = value;
        }

        ValueType GetValue() const 
        {
            return fValue;
        }

    protected:
        virtual EProperty* OnClone() override
        {
            return new EValueProperty(*this);
        }
    };

    class E_API EStructProperty : public EProperty
    {
    private:
        EVector<EProperty*> fProperties;
    public:
        EStructProperty(const EString& name, EValueDescription description, const EVector<EProperty*>& properties = {});
        EStructProperty(const EStructProperty& other);
        ~EStructProperty();

        template <typename T>
        void SetValue(const T& value)
        {
            convert::setter<T>(this, value);
        }

        template <typename T>
        T GetValue() const
        {
            T result;
            convert::getter<T>(this, &result);
            return result;
        }

        bool HasProperty(const EString& propertyName) const;

        EProperty* GetProperty(const EString& propertyName);
        const EProperty* GetProperty(const EString& propertyName) const;

    protected:
        virtual EProperty* OnClone() override;
    };

    class E_API EEnumProperty : public EProperty
    {
    private:
        EString fValue;
    public:
        EEnumProperty(const EString& name, EValueDescription description, const EString& initValue = "");
        EEnumProperty(EEnumProperty&) = default;
        ~EEnumProperty();

        void SetCurrentValue(const EString& value);
        const EString& GetCurrentValue() const;

    protected:
        virtual EProperty* OnClone() override;
    };

    class E_API EArrayProperty : public EProperty
    {
    private:
        EVector<EProperty*> fElements;
    public:
        EArrayProperty(const EString& name, EValueDescription description);
        EArrayProperty(EArrayProperty& other);
        ~EArrayProperty();

        EProperty* AddElement();
        EProperty* GetElement(size_t index);
        void RemoveElement(size_t index);
        const EVector<EProperty*>& GetElements() const;
        void Clear();


    protected:
        virtual EProperty* OnClone() override;
    };

}