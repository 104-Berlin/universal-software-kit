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

// See tests/test_engine_register.cpp
// There is an example for a vector struct right at the top

namespace Engine {

    namespace events {

        template <typename T>
        struct EValueChangeEvent {
            T NewValue;
        };

    }

    class E_API EProperty
    {
        friend class ERegister;
    public:
        EString fName;
        EValueDescription fDescription;
    protected:
        EEventDispatcher fEventDispatcher;
    public:
        EProperty(const EString& name, EValueDescription description);
        virtual ~EProperty() = default;

        const EString& GetPropertyName() const;

        EValueDescription GetDescription() const;

        template <typename Event, typename Callback>
        void AddEventListener(Callback&& cb)
        {
            fEventDispatcher.Connect<Event>(cb);
        }


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

        void SetValue(const ValueType& value)
        {
            fValue = value;
            fEventDispatcher.Post<events::EValueChangeEvent<ValueType>>({value});
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
        EStructProperty(const EString& name, EValueDescription description, const EVector<EProperty*>& properties = {});
        ~EStructProperty();

        template <typename T>
        void SetValue(const T& value)
        {
            convert::setter<T>(this, value);
            fEventDispatcher.Post<events::EValueChangeEvent<T>>({value});
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

    };

    class E_API EEnumProperty : public EProperty
    {
    private:
        EString fValue;
    public:
        EEnumProperty(const EString& name, EValueDescription description, const EString& initValue = "");
        ~EEnumProperty();

        void SetCurrentValue(const EString& value);
        const EString& GetCurrentValue() const;
    };

    class E_API EArrayProperty : public EProperty
    {
    private:
        EVector<EProperty*> fElements;
    public:
        EArrayProperty(const EString& name, EValueDescription description);
        ~EArrayProperty();

        EProperty* AddElement();
        EProperty* GetElement(size_t index);
        void RemoveElement(size_t index);
        const EVector<EProperty*>& GetElements() const;
        void Clear();
    };


    class E_API ERegister
    {
    public:
        using Entity = u32;
    private:
        struct EComponentCreateEvent {
            Entity Entity;
            EStructProperty* Property;
        };
    private:
        EString fName;
        EResourceManager fResourceManager;
        EEventDispatcher fEventDispatcher;

        EUnorderedMap<EValueDescription::t_ID, EUnorderedMap<Entity, EStructProperty*>> fComponentStorage;
        EVector<Entity>     fAliveEntites;
        EVector<Entity>     fDeadEntites;
    public:
        ERegister(const EString& name = "Unknown");
        
        EResourceManager& GetResourceManager();

        Entity CreateEntity();
        void DestroyEntity(Entity entity);
        EVector<Entity> GetAllEntities() const;
        void Clear();

        bool IsAlive(Entity entity);

        void InsertComponent(Entity entity, EValueDescription componentId);
        void RemoveComponent(Entity entity, EValueDescription componentId);
        bool HasComponent(Entity entity, EValueDescription componentId);
        EStructProperty* GetComponent(Entity entity, EValueDescription componentId);
        EVector<EStructProperty*> GetAllComponents(Entity entity);
        EUnorderedMap<Entity, EStructProperty*>& View(const EValueDescription& description);

        template <typename Callback>
        void AddComponentCreateEventListener(const EValueDescription& description, Callback&& cb)
        {
            fEventDispatcher.Connect<EComponentCreateEvent>([cb, description](EComponentCreateEvent evt){
                if (evt.Property->GetDescription() == description)
                {
                    cb(evt.Property);
                }
            });
        }


    };



    namespace events {

        

        struct EComponentChangeEvent {
            ERegister::Entity Entity;
            EStructProperty* Property;
        };

        struct EComponentDeleteEvent {
            ERegister::Entity Entity;
        };

    }

}