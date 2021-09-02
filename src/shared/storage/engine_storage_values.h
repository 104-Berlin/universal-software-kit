#pragma once


namespace Engine {
    class EStructProperty;
}
namespace convert {

    template <typename Value>
    bool setter(Engine::EStructProperty* prop, const Value& value)
    {
        E_ASSERT(false, EString("Setter not implemented for type ") + typeid(Value).name());
        return false;
    }


    template <typename Value>
    bool getter(const Engine::EStructProperty* prop, Value* outValue)
    {
        E_ASSERT(false, EString("Getter not implemented for type ") + typeid(Value).name());
        return false;
    }
};

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
        auto SetValue(const T& value) -> decltype(T::_dsc)
        {
            T::ToProperty(value, this);
        }

        template <typename T>
        void SetValue(const T& value)
        {
            convert::setter(this, value);
        }

        template <typename T>
        auto GetValue(T& outValue) const -> decltype(T::_dsc)
        {
            T result;
            if (T::FromProperty(result, this))
            {
                outValue = result;
                return true;
            }
            return false;
        }

        template <typename T>
        bool GetValue(T& outValue) const
        {
            T result;
            if (convert::getter(this, &result))
            {
                outValue = result;
                return true;
            }
            return false;
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

        template <typename T>
        bool GetValue(EVector<T>& outVector) const
        {
            auto& insert_element = [&outVector, this](auto property){
                T value;
                if (property->template GetValue<T>(value))
                {
                    outVector.push_back(value);
                    return true;
                }                        
                E_ERROR("Getting array property as vector has some type conflicts!");
                E_ERROR("Trying to get array of " + fDescription.GetId() + " as Vector<" + typeid(T).name() + ">");

                return false;
            };

            for (EProperty* prop : fElements)
            {
                switch (fDescription.GetType())
                {
                    case EValueType::STRUCT: 
                    {
                        if (insert_element(static_cast<EStructProperty*>(prop)))
                        {
                            return false;
                        }
                        break;
                    }
                    case EValueType::PRIMITIVE:
                    {
                        if (!insert_element(static_cast<EValueProperty<T>*>(prop)))
                        {
                            return false;
                        }
                        break;
                    } 
                    case EValueType::ENUM:
                    {
                        E_WARN("Array of enums is not supported. Use the enum inside an struct please!");
                        return false;
                        break;
                    }
                    case EValueType::UNKNOWN: return false;
                }
            }
        }
    protected:
        virtual EProperty* OnClone() override;
    };

}