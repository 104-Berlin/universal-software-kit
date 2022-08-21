
#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;




EDataBase::EDataBase(const EString& name) 
    : fName(name)
{
    fResourceManager.GetEventDispatcher().ConnectAll([this](ERef<EProperty> prop){
        this->fEventDispatcher.Enqueue_P(prop->GetDescription(), prop);
    });
}

Engine::EDataBase::~EDataBase() 
{
    fComponentStorage.clear();
}


EResourceManager& EDataBase::GetResourceManager() 
{
    return fResourceManager;
}

EDataBase::Entity EDataBase::CreateEntity() 
{
    static Entity currentEntity = 1;
    
    Entity newEntity = currentEntity;

    if (fDeadEntites.size() > 0)
    {
        newEntity = fDeadEntites.back();
        fDeadEntites.pop_back();
    }
    else
    {
        currentEntity++;
    }
    fAliveEntites.push_back(newEntity);
    EntityChangeEvent event(EntityChangeType::ENTITY_CREATED, newEntity);
    fEventDispatcher.Enqueue<EntityChangeEvent>(event);
    return newEntity;
}

void EDataBase::DestroyEntity(Entity entity) 
{
    EVector<Entity>::iterator it = std::find(fAliveEntites.begin(), fAliveEntites.end(), entity);
    if (it == fAliveEntites.end())
    {
        return;
    }
    for (auto& entry : fComponentStorage)
    {
        entry.second.erase(entity);
    }
    EntityChangeEvent event(EntityChangeType::ENTITY_DESTROYED, entity);
    fEventDispatcher.Enqueue<EntityChangeEvent>(event);
    fAliveEntites.erase(it);
    fDeadEntites.push_back(entity);
}

EVector<EDataBase::Entity> EDataBase::GetAllEntities() const
{
    return fAliveEntites;
}

void EDataBase::Clear() 
{
    fComponentStorage.clear();
    for (auto& entity : fAliveEntites)
    {
        EntityChangeEvent event(EntityChangeType::ENTITY_DESTROYED, entity);
        fEventDispatcher.Enqueue<EntityChangeEvent>(event);
    }

    fAliveEntites.clear();
    fDeadEntites.clear();
}

bool EDataBase::IsAlive(Entity entity) 
{
    return std::find(fAliveEntites.begin(), fAliveEntites.end(), entity) != fAliveEntites.end();
}


EWeakRef<EProperty> EDataBase::AddComponent(Entity entity, const EValueDescription& componentId)
{
    ERef<EProperty> defaultValue = componentId.GetDefaultValue();
    return AddComponent(entity, defaultValue ? defaultValue : EProperty::CreateFromDescription(componentId.GetId(), componentId));
}

EWeakRef<EProperty> EDataBase::AddComponent(Entity entity, const ERef<const EProperty>& value) 
{
    if (!value)
    {
        return {};
    }
    EValueDescription description = value->GetDescription();
    E_ASSERT_M(description.Valid(), "ERROR: Invalid value descrition!");
    E_ASSERT_M(description.GetType() == EValueType::STRUCT, "Component can only be inserted as struct");
    if (!IsAlive(entity)) { return {}; }

    if (!HasComponent(entity, description.GetId()))
    {
        ERef<EProperty> storage = EProperty::CreateFromDescription(description.GetId(), description);
        for (const EValueDescription& depends : description.GetDependsOn())
        {
            AddComponent(entity, depends);
        }
        storage->Copy(value.get());

        storage->SetChangeFunc([this, entity, storage](EString ident){
            EntityChangeEvent event(EntityChangeType::COMPONENT_CHANGED, entity);
            ComponentChangeData data(ident);
            data.NewValue.SetValue(ERef<EProperty>(storage->Clone()));
            ERef<EProperty> newValue = ERef<EProperty>(EProperty::CreateFromDescription(ComponentChangeData::_dsc.GetId(), ComponentChangeData::_dsc));
            convert::setter(newValue.get(), data);
            event.Data.SetValue(newValue);
            fEventDispatcher.Enqueue<EntityChangeEvent>(event);
        });

        fComponentStorage[description.GetId()][entity] = storage;

        EntityChangeEvent event(EntityChangeType::COMPONENT_ADDED, entity);
        event.Data.SetValue(ERef<EProperty>(storage->Clone()));
        fEventDispatcher.Enqueue<EntityChangeEvent>(event);
        return storage;
    }
    return {};
}

void EDataBase::RemoveComponent(Entity entity, const EValueDescription& componentId)
{
    E_ASSERT_M(componentId.Valid(), "ERROR: Invalid value descrition!");
    if (!IsAlive(entity)) { return; }
    if (!HasComponent(entity, componentId)) { return; }

    fComponentStorage[componentId.GetId()].erase(entity);

    EntityChangeEvent event(EntityChangeType::COMPONENT_REMOVED, entity);
    ComponentChangeData data(componentId.GetId());
    ERef<EProperty> newValue = ERef<EProperty>(EProperty::CreateFromDescription(ComponentChangeData::_dsc.GetId(), ComponentChangeData::_dsc));
    convert::setter(newValue.get(), data);
    event.Data.SetValue(newValue);
        
    fEventDispatcher.Enqueue<EntityChangeEvent>(event);
}

bool EDataBase::HasComponent(Entity entity, const EValueDescription& componentId) 
{
    E_ASSERT_M(componentId.Valid(), "ERROR: Invalid value descrition!");
    return HasComponent(entity, componentId.GetId());
}

bool Engine::EDataBase::HasComponent(Entity entity, const EValueDescription::t_ID& componentId) 
{
    return fComponentStorage[componentId].find(entity) != fComponentStorage[componentId].end();
}

EWeakRef<EProperty> EDataBase::GetComponent(Entity entity, const EValueDescription& componentId) 
{
    E_ASSERT_M(componentId.Valid(), "ERROR: Invalid value descrition!");
    return GetComponent(entity, componentId.GetId());
}

EWeakRef<EProperty> EDataBase::GetComponent(Entity entity, const EValueDescription::t_ID& componentId) 
{
    if (!IsAlive(entity)) { return ERef<EProperty>(nullptr); }
    if (!HasComponent(entity, componentId)) { return ERef<EProperty>(nullptr); }

    return fComponentStorage[componentId][entity];
}

void Engine::EDataBase::DisconnectEvents() 
{
    fEventDispatcher.DisconnectEvents();
}

void Engine::EDataBase::WaitForEvent() 
{
    fEventDispatcher.WaitForEvent();
}

EEventDispatcher& Engine::EDataBase::GetEventDispatcher()
{
    return fEventDispatcher;
}

void Engine::EDataBase::UpdateEvents() 
{
    fEventDispatcher.Update();
}

EWeakRef<EProperty> EDataBase::GetValueByIdentifier(Entity entity, const EString& identifier) 
{
    EVector<EString> identList;
    size_t start = 0;
    size_t end = identifier.find(".");
    while (end != EString::npos)
    {
        identList.push_back(identifier.substr(start, end - start));
        start = end + 1;
        end = identifier.find(".", start);
    }
    identList.push_back(identifier.substr(start, identifier.length() - start));
    E_ASSERT(identList.size() > 0);
    // The first identifier is the component name
    EWeakRef<EProperty> currentProp = GetComponent(entity, identList[0]);
    // if we couldnt find the component return
    if (!currentProp.lock()) { return currentProp; }
    if (identList.size() == 1) { return currentProp; }
    if (currentProp.lock()->GetDescription().GetType() != EValueType::STRUCT) { return currentProp; }
    EWeakRef<EStructProperty> asStructProp = std::dynamic_pointer_cast<EStructProperty>(currentProp.lock());
    return asStructProp.lock()->GetPropertyByIdentifier(identifier.substr(identList[0].length() + 1));
}

EVector<ERef<EProperty>> EDataBase::GetAllComponents(Entity entity) 
{
    EVector<ERef<EProperty>> result;
    for (auto& entry : fComponentStorage)
    {
        if (entry.second.find(entity) != entry.second.end())
        {
            result.push_back(entry.second[entity]);
        }
    }
    return result;
}

EUnorderedMap<EDataBase::Entity, ERef<EProperty>>& EDataBase::View(const EValueDescription& description) 
{
    return fComponentStorage[description.GetId()];
}