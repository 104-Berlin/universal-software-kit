#pragma once



// See tests/test_engine_register.cpp
// There is an example for a vector struct right at the top

namespace Engine {


    class E_API EDataBase
    {
    public:
        using Entity = u64;
    private:
    private:
        EString fName;
        EResourceManager fResourceManager;
        EEventDispatcher fEventDispatcher;

        EUnorderedMap<EValueDescription::t_ID, EUnorderedMap<Entity, EStructProperty*>> fComponentStorage;
        EVector<Entity>     fAliveEntites;
        EVector<Entity>     fDeadEntites;
    public:
        EDataBase(const EString& name = "Unknown");
        ~EDataBase();
        
        EResourceManager& GetResourceManager();

        Entity CreateEntity();
        void DestroyEntity(Entity entity);
        EVector<Entity> GetAllEntities() const;
        void Clear();

        bool IsAlive(Entity entity);

        EStructProperty* AddComponent(Entity entity, const EValueDescription& componentId);
        void RemoveComponent(Entity entity, const EValueDescription& componentId);
        bool HasComponent(Entity entity, const EValueDescription& componentId);
        bool HasComponent(Entity entity, const EValueDescription::t_ID& componentId);
        EStructProperty* GetComponent(Entity entity, const EValueDescription& componentId);
        EStructProperty* GetComponent(Entity entity, const EValueDescription::t_ID& componentId);
        EProperty* GetValueByIdentifier(Entity entity, const EString& identifier);
        EVector<EStructProperty*> GetAllComponents(Entity entity);
        EUnorderedMap<Entity, EStructProperty*>& View(const EValueDescription& description);


        template <typename T>
        T AddComponent(Entity entity)
        {
            EStructProperty* inserted = AddComponent(entity, getdsc::GetDescription<T>());
            T result;
            if (inserted)
            {
                convert::getter<T>(inserted, &result);
            }
            return result;
        }

        template <typename T>
        T GetComponent(Entity entity)
        {
            T result;
            convert::getter<T>(GetComponent(entity, getdsc::GetDescription<T>()), &result);
            return result;
        }



        template <typename Callback>
        void CatchAllEvents(Callback cb)
        {
            fEventDispatcher.ConnectAll(cb);
        }


        // Do these between extension deletion and scene delete. 
        // If a lambda is defined in an extension the event dispatcher cant clean up the lambda, because the symbols are not loaded anymore
        void DisconnectEvents();
        void WaitForEvent();

        EEventDispatcher& GetEventDispatcher();

        void UpdateEvents();
    };

    E_STORAGE_STRUCT(EntityHandle,
        (EDataBase::Entity, Handle)
    )

    E_STORAGE_STRUCT(ComponentChangeData,
        (EString, Identifier),
        (EAny, NewValue)
    )

    E_STORAGE_ENUM(EntityChangeType,
        ENTITY_CREATED,
        ENTITY_DESTROYED,
        COMPONENT_ADDED,
        COMPONENT_REMOVED,
        COMPONENT_CHANGED
    )

    E_STORAGE_STRUCT(EntityChangeEvent,
        (EntityChangeType, Type),
        (EntityHandle, Entity),
        (EAny, Data)
    )

}