#pragma once



// See tests/test_engine_register.cpp
// There is an example for a vector struct right at the top

namespace Engine {



    class E_API ERegister
    {
    public:
        using Entity = u64;
        E_STORAGE_STRUCT(ComponentCreateEvent, 
            (EValueDescription::t_ID, ValueId),
            (Entity, Handle)
        )

        E_STORAGE_STRUCT(ValueChangeEvent,
            (EString, Identifier),
            (Entity, Handle)
        )
    private:
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

        void InsertComponent(Entity entity, const EValueDescription& componentId);
        void RemoveComponent(Entity entity, const EValueDescription& componentId);
        bool HasComponent(Entity entity, const EValueDescription& componentId);
        bool HasComponent(Entity entity, const EValueDescription::t_ID& componentId);
        EStructProperty* GetComponent(Entity entity, const EValueDescription& componentId);
        EStructProperty* GetComponent(Entity entity, const EValueDescription::t_ID& componentId);
        EProperty* GetValueByIdentifier(Entity entity, const EString& identifier);
        EVector<EStructProperty*> GetAllComponents(Entity entity);
        EUnorderedMap<Entity, EStructProperty*>& View(const EValueDescription& description);

        template <typename Callback>
        void AddComponentCreateEventListener(const EValueDescription& description, Callback&& cb)
        {
            fEventDispatcher.Connect<ComponentCreateEvent>([cb, description](ComponentCreateEvent event){
                if (event.ValueId == description.GetId())
                {
                    cb(event.Handle);
                }
            });
        }

        template <typename Callback>
        void AddEntityChangeEventListener(const EString& valueIdent, Callback&& cb)
        {
            fEventDispatcher.Connect<ValueChangeEvent>([cb, valueIdent](ValueChangeEvent event){
                if (event.Identifier.length() < valueIdent.length()) {return;}
                if (valueIdent == event.Identifier.substr(0, valueIdent.length()))
                {
                    cb(event.Handle, valueIdent);
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