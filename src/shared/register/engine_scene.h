#pragma once

template <typename Property, typename Value>
struct convert {
    static void setter(Property* prop, const Value& value)
    {
        E_ASSERT(false, EString("Setter not implemented for type ") + typeid(Value).name());
    }

    static void getter(const Property* prop, Value* outValue)
    {
        E_ASSERT(false, EString("Getter not implemented for type ") + typeid(Value).name());
    }
};

// See tests/test_engine_register.cpp
// There is an example for a vector struct right at the top

namespace Engine {

    class E_API EProperty
    {
        friend class EScene;
    public:
        EString fName;
        ERef<EValueDescription> fDescription;
    public:
        EProperty(const EString& name, ERef<EValueDescription> description);
        virtual ~EProperty() = default;

        const EString& GetPropertyName() const;

        ERef<EValueDescription> GetDescription() const;


        
        static EProperty* CreateFromDescription(const EString& name, ERef<EValueDescription> description);
    private:
        static EProperty* CreatePropertyStruct(const EString& name, ERef<EStructDescription> description);
        static EProperty* CreatePropertyPrimitive(const EString& name, ERef<EValueDescription> descrption);
        static EProperty* CreatePropertyEnum(const EString& name, ERef<EEnumDescription> descrption);
        static EProperty* CreatePropertyArray(const EString& name, ERef<EArrayDescription> description);
    };

    template <typename ValueType>
    class EValueProperty : public EProperty
    {
    private:
        ValueType fValue;
    public:
        EValueProperty(const EString& name, ERef<EValueDescription> description, const ValueType& initValue = ValueType())
            : EProperty(name, description)
        {
            fValue = initValue;
        }

        void SetValue(const ValueType& value)
        {
            fValue = value;
        }

        ValueType GetValue() const 
        {
            return fValue;
        }
    };

    class E_API EStructProperty : public EProperty
    {
    private:
        EVector<EProperty*> fProperties;
    public:
        EStructProperty(const EString& name, ERef<EStructDescription> description, const EVector<EProperty*>& properties = {});
        ~EStructProperty();

        template <typename T>
        void SetValue(const T& value)
        {
            convert<EStructProperty, T>::setter(this, value);
        }

        template <typename T>
        T GetValue() const
        {
            T result;
            convert<EStructProperty, T>::getter(this, &result);
            return result;
        }

        bool HasProperty(const EString& propertyName) const;

        EProperty* GetProperty(const EString& propertyName);
        const EProperty* GetProperty(const EString& propertyName) const;

    };

    class E_API EEnumProperty : public EProperty
    {
    private:
        EString fValue;
    public:
        EEnumProperty(const EString& name, ERef<EEnumDescription> description, const EString& initValue = "");
        ~EEnumProperty();

        void SetCurrentValue(const EString& value);
        const EString& GetCurrentValue() const;
    };

    class E_API EArrayProperty : public EProperty
    {
    private:
        EVector<EProperty*> fElements;
    public:
        EArrayProperty(const EString& name, ERef<EArrayDescription> description);
        ~EArrayProperty();

        EProperty* AddElement();
        EProperty* GetElement(size_t index);
        void RemoveElement(size_t index);
        const EVector<EProperty*>& GetElements() const;
        void Clear();
    };


    class E_API EScene
    {
    public:
        using Entity = u32;
    private:
        EString fName;
        EResourceManager fResourceManager;

        EUnorderedMap<ERef<EValueDescription>, EUnorderedMap<Entity, EStructProperty*>> fComponentStorage;
        EVector<Entity>     fAliveEntites;
        EVector<Entity>     fDeadEntites;
    public:
        EScene(const EString& name = "Unknown");
        
        EResourceManager& GetResourceManager();

        Entity CreateEntity();
        void DestroyEntity(Entity entity);
        EVector<Entity> GetAllEntities() const;
        void Clear();

        bool IsAlive(Entity entity);

        void InsertComponent(Entity entity, ERef<EValueDescription> componentId);
        void RemoveComponent(Entity entity, ERef<EValueDescription> componentId);
        bool HasComponent(Entity entity, ERef<EValueDescription> componentId);
        EStructProperty* GetComponent(Entity entity, ERef<EValueDescription> componentId);
        EVector<EStructProperty*> GetAllComponents(Entity entity);
    };

}