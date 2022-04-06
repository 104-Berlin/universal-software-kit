#pragma once

namespace Engine {
    class EProperty;
    class EStructProperty;

    template <typename T>
    class EValueProperty;
}
namespace convert {

    template <typename Value>
    bool setter(Engine::EProperty* prop, const Value& value)
    {
        if constexpr (is_primitive<Value>)
        {
            ((Engine::EValueProperty<Value>*)prop)->SetValue(value);
            return true;
        }
        else
        {
            return Value::ToProperty(value, (Engine::EStructProperty*)prop);
        }
    }


    template <typename Value>
    bool getter(const Engine::EProperty* prop, Value* outValue)
    {
        if constexpr (is_primitive<Value>)
        {
            *outValue = ((Engine::EValueProperty<Value>*)prop)->GetValue();
            return true;
        }
        else
        {
            return Value::FromProperty(*outValue,(Engine::EStructProperty*) prop);
        }
    }
};

namespace Engine {
    

    class E_API EProperty
    {
        friend class EDataBase;
        friend class EAny;
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
        EProperty(const EProperty&) = default;
        virtual ~EProperty() = default;

        const EString& GetPropertyName() const;

        EValueDescription GetDescription() const;

        ERef<EProperty> Clone() const;
        void Copy(const EProperty* from);

        
        bool IsValid() const;
        bool operator()() const;
    protected:
        virtual void OnCopy(const EProperty* from) = 0;
        virtual ERef<EProperty> OnClone() const = 0;
        // Only used by register. Should stay this way
        // Register sends the events then
        void SetChangeFunc(ChangeFunc func);
        void ConnectChangeFunc(EProperty* other);
    public:
        static void Copy(const EProperty* copyFrom, EProperty* copyTo);

        static ERef<EProperty> CreateFromDescription(const EString& name, EValueDescription description);

        template <typename T>
        static ERef<EProperty> CreateFromTemplate(const EString& name)
        {
            ERef<EProperty> result = CreateFromDescription(name, getdsc::GetDescription<T>());
            if (result)
            {
                T initValue;
                convert::setter<T>(result.get(), initValue);
            }
            return result;
        }

    private:
        static ERef<EProperty> CreatePropertyStruct(const EString& name, EValueDescription description);
        static ERef<EProperty> CreatePropertyPrimitive(const EString& name, EValueDescription descrption);
        static ERef<EProperty> CreatePropertyEnum(const EString& name, EValueDescription descrption);
        static ERef<EProperty> CreatePropertyArray(const EString& name, EValueDescription description);
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

        EValueProperty(const EValueProperty&) = default;

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

        template <typename T>
        bool GetValue(T& outValue) const
        {
            if (typeid(T) != typeid(ValueType))
            {
                return false;
            }
            outValue = (T)fValue;
            return true;
        }

    protected:
        virtual ERef<EProperty> OnClone() const override
        {
            return EMakeRef<EValueProperty>(*this);
        }

        virtual void OnCopy(const EProperty* from) override
        {
            fValue = static_cast<const EValueProperty<ValueType>*>(from)->fValue;
        }
    };

    class E_API EStructProperty : public EProperty
    {
        friend class EAny;
    private:
        EVector<ERef<EProperty>> fProperties;
    public:
        EStructProperty(const EString& name, EValueDescription description, const EVector<ERef<EProperty>>& properties = {});
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

        ERef<EProperty> GetProperty(const EString& propertyName);
        const ERef<EProperty> GetProperty(const EString& propertyName) const;

        ERef<EProperty> GetPropertyByIdentifier(const EString& ident) const;
    private:
        /**
         * @brief Clears the fields and deletes them
         */
        void ResetFields();
    protected:
        virtual ERef<EProperty> OnClone() const override;
        virtual void OnCopy(const EProperty* from) override;
    };

    
    // Enum property member
    HAS_MEMBER(Value)


    class E_API EEnumProperty : public EProperty
    {
    private:
        u32 fValue;
    public:
        EEnumProperty(const EString& name, EValueDescription description, const EString& initValue = "");
        EEnumProperty(const EEnumProperty&) = default;
        ~EEnumProperty();

        u32 GetCurrentValue() const;
        
        /**
         * @brief This is used for the STORAGE Macro to work
         * 
         * @tparam T - The type of the enum. Created with E_STORAGE_ENUM (...) see engine_storage_macro.h
         */
        template <typename T>
        bool GetCurrentValue(T& outValue) const
        {
            if constexpr (has_member_Value <T>::value)
            {
                outValue.Value = (typename T::opts)fValue;
                return true;
            }
            return false;
        }

        template <typename T>
        bool SetCurrentValue(const T& Value)
        {
            if constexpr (has_member_Value <T>::value)
            {
                SetCurrentValueIndex(static_cast<u32>(Value.Value));
                return true;
            }
            return false;
        }
        

        bool SetCurrentValueOption(const EString& value) 
        {
            if (value.empty())
            {
                fValue = 0;
                return false;
            }
            EValueDescription dsc = GetDescription();
            E_ASSERT(dsc.GetType() == EValueType::ENUM);
            const EVector<EString>& options = dsc.GetEnumOptions();
            EVector<EString>::const_iterator foundOption = std::find(options.begin(), options.end(), value);
            if (foundOption == options.end())
            {
                fValue = 0;
            }
            else
            {
                fValue = std::distance(options.begin(), foundOption);
            }

            if (fChangeFunc) { fChangeFunc(GetPropertyName()); }
            return true;
        }

        bool SetCurrentValueIndex(const u32& value)
        {
            EValueDescription dsc = GetDescription();
            E_ASSERT(dsc.GetType() == EValueType::ENUM);
            const EVector<EString>& options = dsc.GetEnumOptions();
            if (value >= options.size())
            {
                fValue = 0;
            }
            else
            {
                fValue = value;
            }

            if (fChangeFunc) { fChangeFunc(GetPropertyName()); }
            return true;
        }

    protected:
        virtual ERef<EProperty> OnClone() const override;
        virtual void OnCopy(const EProperty* from) override;
    };

    class E_API EArrayProperty : public EProperty
    {
    private:
        EVector<ERef<EProperty>> fElements;
    public:
        EArrayProperty(const EString& name, EValueDescription description);
        EArrayProperty(const EArrayProperty& other);
        ~EArrayProperty();

        ERef<EProperty> AddElement();
        ERef<EProperty> GetElement(size_t index);
        void RemoveElement(size_t index);
        const EVector<ERef<EProperty>>& GetElements() const;
        void Clear();

        template <typename T>
        bool GetValue(T& outVector) const
        {
            if constexpr (!is_vector<T>::value)
            {
                return false;
            }
            else
            {
                using ArrayType = typename T::value_type;

                auto& insert_element = [&outVector, this](auto property)mutable{
                    typename T::value_type value;
                    if (property->GetValue<typename T::value_type>(value))
                    {
                        outVector.push_back(value);
                        return true;
                    }                  
                    E_ERROR("Getting array property as vector has some type conflicts!");
                    E_ERROR("Trying to get array of " + fDescription.GetId() + " as Vector<" + typeid(T::value_type).name() + ">");

                    return false;
                };

                EValueDescription dsc = fDescription.GetAsPrimitive();
                for (ERef<EProperty> prop : fElements)
                {
                    switch (dsc.GetType())
                    {
                        case EValueType::ANY:
                        case EValueType::STRUCT: 
                        {
                            if (!insert_element(std::static_pointer_cast<EStructProperty>(prop)))
                            {
                                return false;
                            }
                            break;
                        }
                        case EValueType::ARRAY:
                        {
                            if (!insert_element(std::static_pointer_cast<EArrayProperty>(prop)))
                            {
                                return false;
                            }
                            break;
                        }
                        case EValueType::PRIMITIVE:
                        {
                            if (!insert_element(std::static_pointer_cast<EValueProperty<typename T::value_type>>(prop)))
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
            return true;
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
            for (const ArrayType& entry : vector)
            {
                size_t currentIndex = fElements.size();
                ERef<EProperty> newEntry = EProperty::CreateFromDescription(std::to_string(currentIndex), dsc);
                ConnectChangeFunc(newEntry.get());
                switch (dsc.GetType())
                {
                case EValueType::PRIMITIVE:
                {
                    std::static_pointer_cast<EValueProperty<typename T::value_type>>(newEntry)->SetValue(entry);
                    break;
                }
                case EValueType::ANY:
                case EValueType::STRUCT:
                {
                    if (!std::static_pointer_cast<EStructProperty>(newEntry)->SetValue<typename T::value_type>(entry))
                    {
                        return false;
                    }
                    break;
                }
                case EValueType::ARRAY:
                {
                    if constexpr (is_vector<ArrayType>::value)
                    {
                        if (!std::static_pointer_cast<EArrayProperty>(newEntry)->SetValue<typename T::value_type>(entry))
                        {
                            return false;
                        }
                    }
                    else
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
        virtual ERef<EProperty> OnClone() const override;
        virtual void OnCopy(const EProperty* from) override;
    };


    // The any type. Can be used as basic example for own types

    class E_API EAny
    {
    private:
        ERef<EProperty> fProperty;
    public:
        EAny() : fProperty(nullptr) {}
        EAny(const EAny& other) = default;
        ~EAny() = default;

        EAny& operator=(const EAny& other) = default;

        bool operator==(const EAny& other) const
        {
            if (!fProperty || !other.fProperty) { return fProperty == other.fProperty; }
            return fProperty->GetDescription() == other.fProperty->GetDescription();
        }

        bool operator!=(const EAny& other) const
        {
            return !(*this == other);
        }

        static bool ToProperty(const EAny& value, ::Engine::EProperty* property)
        {
            E_ASSERT_M(property->GetDescription().GetType() == EValueType::ANY, "Trying to set a value to a property that is not of type ANY!");
            if (property->GetDescription().GetType() != EValueType::ANY) { return false; }
            ::Engine::EStructProperty* structProperty = static_cast<::Engine::EStructProperty*>(property);
            structProperty->ResetFields();

            if (value.fProperty)
            {
                ERef<EProperty> newValueProperty = EProperty::CreateFromDescription("value", value.fProperty->GetDescription());
                newValueProperty->Copy(value.fProperty.get());
                structProperty->fProperties.push_back(newValueProperty);
            }
            return true;
        }

        static bool FromProperty(EAny& value, const ::Engine::EProperty* property)
        {
            EValueDescription dsc = property->GetDescription();
            if (dsc.GetType() != EValueType::ANY) { return false; }
            const ::Engine::EStructProperty* structProperty = static_cast<const ::Engine::EStructProperty*>(property);
            
            const ERef<EProperty> valueProperty = structProperty->GetProperty("value");

            if (valueProperty)
            {
                value.fProperty = ERef<EProperty>(EProperty::CreateFromDescription("value", valueProperty->GetDescription()));
                value.fProperty->Copy(valueProperty.get());
            }
            else
            {
                value.fProperty = nullptr;
            }
            return true;
        }

        void SetValue(const ERef<EProperty>& property)
        {
            fProperty = property;
        }

        ERef<EProperty> Value() const
        {
            return fProperty;
        }

    public:
        static inline EValueDescription _dsc = EValueDescription(EValueType::ANY/*In case of own struct use EValueType::STRUCT*/, "Any");
    };

}