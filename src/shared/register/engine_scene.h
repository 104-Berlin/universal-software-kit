#pragma once

namespace Engine {

    enum class EValueType
    {
        INTEGER,
        DOUBLE,
        BOOL,
        STRING,

        STRUCT,

        COMPONENT_REFERENCE,
    };

    struct EComponentTypeDescription
    {
        EValueType      Type;
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

    class EStructProperty : public EProperty
    {
    private:
        EVector<EProperty*> fProperties;
    public:
        EStructProperty(const EString& name, const EVector<EProperty*>& property);
        ~EStructProperty();


    };


    class EComponentStorage
    {
    private:
        EComponentDescription   fDsc;
        EUnorderedMap<EString, EProperty*>     fProperties;
    public:
        EComponentStorage(EComponentDescription dsc = EComponentDescription(), const EUnorderedMap<EString, EProperty*>& propInit = {});

        ~EComponentStorage();

        operator bool() const;
        bool Valid() const;


        void Reset();

        bool HasProperty(const EString& propertyName);

        bool GetProperty(const EString& propertyName, EValueProperty<i32>** outValue);
        bool GetProperty(const EString& propertyName, EValueProperty<double>** outValue);
        bool GetProperty(const EString& propertyName, EValueProperty<bool>** outValue);
        bool GetProperty(const EString& propertyName, EValueProperty<EString>** outValue);
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