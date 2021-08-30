#pragma once



// See tests/test_engine_register.cpp
// There is an example for a vector struct right at the top

namespace Engine {



    class E_API ERegister
    {
    public:
        using Entity = u32;
    private:
        struct EComponentCreateEvent {
            Entity EntityID;
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
            /*fEventDispatcher.Connect<EComponentCreateEvent>([cb, description](EComponentCreateEvent evt){
                if (evt.Property->GetDescription() == description)
                {
                    cb(evt.Property, evt.EntityID);
                }
            });*/
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