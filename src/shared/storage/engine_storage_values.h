#pragma once

namespace Engine {
    class EStructProperty;
}
namespace convert {

    template <typename Value>
    bool setter(Engine::EStructProperty* prop, const Value& value)
    {
        if constexpr (is_primitive<Value>)
        {
            return false;
        }
        else
        {
            return Value::ToProperty(value, prop);
        }
    }


    template <typename Value>
    bool getter(const Engine::EStructProperty* prop, Value* outValue)
    {
        if constexpr (is_primitive<Value>)
        {
            return false;
        }
        else
        {
            return Value::FromProperty(*outValue, prop);
        }
    }
};

namespace Engine {
    

    class E_API EProperty
    {
        friend class ERegister;
        // Function when value changes
        // Contains string to the current property identifier
        using ChangeFunc = std::function<void(EString)>;
    public:
        EString fName;
        EValueDescription fDescription;
    protected:
        ChangeFunc fChangeFunc;
    public:
        EProperty(const EString& name, EValueDescription description);
        EProperty(EProperty&) = default;
        virtual ~EProperty() = default;

        const EString& GetPropertyName() const;

        EValueDescription GetDescription() const;

        EProperty* Clone();
        
        bool IsValid() const;
        bool operator()() const;
    protected:
        virtual EProperty* OnClone() = 0;
        // Only used by register. Should stay this way
        // Register sends the events then
        void SetChangeFunc(ChangeFunc func);
        void ConnectChangeFunc(EProperty* other);
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
            if (fChangeFunc) { fChangeFunc(GetPropertyName()); }
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
        bool SetValue(const T& value) 
        {
            if (convert::setter<T>(this, value))
            {
                if (fChangeFunc) { fChangeFunc(GetPropertyName()); }
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
        auto GetValue(EVector<T>& outVector) const
        -> std::enable_if_t<is_vector<T>::value, bool>
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
                        if (!insert_element(static_cast<EStructProperty*>(prop)))
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

        template <typename T>
        bool GetValue(T& outVector) const
        {
            return false;
        }

        template <typename T>
        bool SetValue(const T& vector)
        {
            if constexpr (!is_vector<T>::value)
            {
                return false;
            }
            using ArrayType = typename T::value_type;
            Clear();
            EValueDescription dsc = fDescription.GetAsPrimitive();
            for (const typename T::value_type& entry : vector)
            {
                size_t currentIndex = fElements.size();
                EProperty* newEntry = EProperty::CreateFromDescription(std::to_string(currentIndex), dsc);
                ConnectChangeFunc(newEntry);
                switch (dsc.GetType())
                {
                case EValueType::PRIMITIVE:
                {
                    static_cast<EValueProperty<typename T::value_type>*>(newEntry)->SetValue(entry);
                    break;
                }
                case EValueType::STRUCT:
                {
                    if (!static_cast<EStructProperty*>(newEntry)->SetValue<typename T::value_type>(entry))
                    {
                        return false;
                    }
                    break;
                }
                case EValueType::ENUM:
                {
                    // TODO:
                    break;
                }
                case EValueType::UNKNOWN:
                {
                    break;
                }
                }
                fElements.push_back(newEntry);
            }
            if (fChangeFunc) {fChangeFunc(GetPropertyName());}
            return true;
        }
    protected:
        virtual EProperty* OnClone() override;
    };

}