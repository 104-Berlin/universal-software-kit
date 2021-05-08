#pragma once

namespace Engine {

    enum class EComponentType
    {
        INTEGER,
        DOUBLE,
        BOOL,
        STRING,

        COMPONENT_REFERENCE,
    };

    struct EComponentTypeDescription
    {
        EComponentType  Type;
        EString         Name;
    };
    typedef EVector<EComponentTypeDescription> TComponentTypeList;

    struct EComponentDescription
    {
        using ComponentID = EString;

        TComponentTypeList  TypeDesciptions;
        ComponentID         ID;

        EComponentDescription(const ComponentID& id = "Unknown", std::initializer_list<EComponentTypeDescription>&& types = {})
            : TypeDesciptions(types), ID(id)
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

        bool GetTypeDescription(const EString& name, EComponentTypeDescription* outDesc)
        {
            for (const EComponentTypeDescription& dsc : TypeDesciptions)
            {
                if (dsc.Name == name)
                {
                    *outDesc = dsc;
                    return true;
                }
            }
            return false;
        }
    private:
        friend class EScene;
    };

    E_API size_t GetComponentTypeSize(EComponentType type);
    E_API size_t GetComponentSize(EComponentDescription description);



    class E_API EProperty
    {
        friend class EScene;
    public:
        EString fName;
    public:
        EProperty(const EString& name);
        virtual ~EProperty() = default;

        const EString& GetPropertyName() const;
    };

    template <typename ValueType>
    class EValueProperty : public EProperty
    {
    private:
        ValueType fValue;
    public:
        EValueProperty(const EString& name, const ValueType& initValue = ValueType())
            : EProperty(name)
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


    class EComponentStorage
    {
    private:
        EComponentDescription   fDsc;
        EUnorderedMap<EString, EProperty*>     fProperties;
    public:
        EComponentStorage(EComponentDescription dsc = EComponentDescription(), const EUnorderedMap<EString, EProperty*>& propInit = {})
            : fDsc(dsc), fProperties(propInit)
            {}

        ~EComponentStorage()
        {
            fProperties.clear();
        }

        operator bool() const
        {
            return Valid();
        }

        bool Valid() const
        {
            return fDsc.Valid();
        }

        void Reset()
        {
            for (auto& property : fProperties)
            {
                delete property.second;
            }
        }

        bool HasProperty(const EString& propertyName)
        {
            return fProperties.find(propertyName) != fProperties.end();
        }

        template <typename T>
        T* GetProperty(const EString& propertyName)
        {
            E_ERROR("Unknown property type to get from componentstorage");
        }

        template <>
        EValueProperty<i32>* GetProperty(const EString& propertyName)
        {
            if (!HasProperty(propertyName))
            {
                return nullptr;
            }
            EComponentTypeDescription typeDsc;
            if (!fDsc.GetTypeDescription(propertyName, &typeDsc))
            {
                E_WARN("Proeprty with name not added. Reset the component!");
                return nullptr;
            }
            if (typeDsc.Type != EComponentType::INTEGER)
            {
                E_ERROR("Wrong Type getting property " + propertyName);
                return nullptr;
            }
            return (EValueProperty<i32>*) fProperties[propertyName];
        }

        template <>
        EValueProperty<double>* GetProperty(const EString& propertyName)
        {
            if (!HasProperty(propertyName))
            {
                return nullptr;
            }
            EComponentTypeDescription typeDsc;
            if (!fDsc.GetTypeDescription(propertyName, &typeDsc))
            {
                E_WARN("Proeprty with name not added. Reset the component!");
                return nullptr;
            }
            if (typeDsc.Type != EComponentType::DOUBLE)
            {
                E_ERROR("Wrong Type getting property " + propertyName);
                return nullptr;
            }
            return (EValueProperty<double>*) fProperties[propertyName];
        }

        template <>
        EValueProperty<bool>* GetProperty(const EString& propertyName)
        {
            if (!HasProperty(propertyName))
            {
                return nullptr;
            }
            EComponentTypeDescription typeDsc;
            if (!fDsc.GetTypeDescription(propertyName, &typeDsc))
            {
                E_WARN("Proeprty with name not added. Reset the component!");
                return nullptr;
            }
            if (typeDsc.Type != EComponentType::BOOL)
            {
                E_ERROR("Wrong Type getting property " + propertyName);
                return nullptr;
            }
            return (EValueProperty<bool>*) fProperties[propertyName];
        }

        template <>
        EValueProperty<EString>* GetProperty(const EString& propertyName)
        {
            if (!HasProperty(propertyName))
            {
                return nullptr;
            }
            EComponentTypeDescription typeDsc;
            if (!fDsc.GetTypeDescription(propertyName, &typeDsc))
            {
                E_WARN("Proeprty with name not added. Reset the component!");
                return nullptr;
            }
            if (typeDsc.Type != EComponentType::STRING)
            {
                E_ERROR("Wrong Type getting property " + propertyName);
                return nullptr;
            }
            return (EValueProperty<EString>*) fProperties[propertyName];
        }
    };

    class E_API EScene
    {
    public:
        using Entity = u32;
    private:
        EString fName;
        EResourceManager fResourceManager;

        EUnorderedMap<EComponentDescription::ComponentID, EComponentDescription> fRegisteredComponents;        

        EUnorderedMap<EComponentDescription::ComponentID, EUnorderedMap<Entity, EComponentStorage>> fComponentStorage;
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

        EResourceManager& GetResourceManager();

        Entity CreateEntity();
        void DestroyEntity(Entity entity);

        bool IsAlive(Entity entity);

        void InsertComponent(Entity entity, EComponentDescription::ComponentID componentId);
        void RemoveComponent(Entity entity, EComponentDescription::ComponentID componentId);
        bool HasComponent(Entity entity, EComponentDescription::ComponentID componentId);
        EComponentStorage GetComponent(Entity entity, EComponentDescription::ComponentID componentId);
    };

}