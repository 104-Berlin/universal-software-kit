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

namespace Engine {

    struct EComponentDescription
    {
        using ComponentID = EString;

        ComponentID         ID;

        EComponentDescription(const ComponentID& id = "")
            : ID(id)
        {}

        EComponentDescription(const EComponentDescription&) = default;

        operator bool() const
        {
            return Valid();
        }

        bool Valid() const
        {
            return !ID.empty();
        }

    private:
        friend class EScene;
    };


    class E_API EProperty
    {
        friend class EScene;
    public:
        EString fName;
        EValueDescription* fDescription;
    public:
        EProperty(const EString& name, EValueDescription* description);
        virtual ~EProperty() = default;

        const EString& GetPropertyName() const;

        EValueDescription* GetDescription() const;
    };

    template <typename ValueType>
    class EValueProperty : public EProperty
    {
    private:
        ValueType fValue;
    public:
        EValueProperty(const EString& name, EValueDescription* description, const ValueType& initValue = ValueType())
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
        EStructProperty(const EString& name, EStructDescription* description, const EVector<EProperty*>& properties = {});
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


    class E_API EScene
    {
    public:
        using Entity = u32;
    private:
        EString fName;
        EResourceManager fResourceManager;

        EUnorderedMap<EComponentDescription::ComponentID, EComponentDescription> fRegisteredComponents;        

        EUnorderedMap<EComponentDescription::ComponentID, EUnorderedMap<Entity, EStructProperty*>> fComponentStorage;
        EVector<Entity>     fAliveEntites;
        EVector<Entity>     fDeadEntites;
    public:
        EScene(const EString& name = "Unknown");
        /**
         * Register a new component.
         * These can be instantiated with an object handle to the data
         * @param description The component description
         */
        void RegisterComponent(EComponentDescription description);

        EVector<EComponentDescription> GetRegisteredComponents() const;

        EResourceManager& GetResourceManager();

        Entity CreateEntity();
        void DestroyEntity(Entity entity);
        EVector<Entity> GetAllEntities() const;

        bool IsAlive(Entity entity);

        void InsertComponent(Entity entity, EComponentDescription::ComponentID componentId);
        void RemoveComponent(Entity entity, EComponentDescription::ComponentID componentId);
        bool HasComponent(Entity entity, EComponentDescription::ComponentID componentId);
        EStructProperty* GetComponent(Entity entity, EComponentDescription::ComponentID componentId);
        EVector<EStructProperty*> GetAllComponents(Entity entity);

    private:
        EProperty* CreatePropertyFromDescription(const EString& name, EValueDescription* description);
        EProperty* CreatePropertyStruct(const EString& name, EStructDescription* description);
        EProperty* CreatePropertyPrimitive(const EString& name, EValueDescription* descrption);
    };

}