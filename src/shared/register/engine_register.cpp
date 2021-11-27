
#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;




EDataBase::EDataBase(const EString& name) 
    : fName(name)
{
    fResourceManager.GetEventDispatcher().ConnectAll([this](EProperty* prop){
        this->fEventDispatcher.Enqueue_P(prop->GetDescription(), prop);
    });
}

Engine::EDataBase::~EDataBase() 
{
    for (auto& entry : fComponentStorage)
    {
        for (auto& values : entry.second)
        {
            delete values.second;
        }
    }
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
    fEventDispatcher.Enqueue<EntityCreateEvent>({newEntity});
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
        fEventDispatcher.Enqueue<ComponentDeleteEvent>({entry.first, entity});
        delete entry.second[entity];
        entry.second.erase(entity);
    }
    fAliveEntites.erase(it);
    fDeadEntites.push_back(entity);
}

EVector<EDataBase::Entity> EDataBase::GetAllEntities() const
{
    return fAliveEntites;
}

void EDataBase::Clear() 
{
    for (auto& entry : fComponentStorage)
    {
        for (auto& storage : entry.second)
        {
            fEventDispatcher.Enqueue<ComponentDeleteEvent>({entry.first, storage.first});
            delete storage.second;
        }
    }
    fComponentStorage.clear();
    fAliveEntites.clear();
    fDeadEntites.clear();
}

bool EDataBase::IsAlive(Entity entity) 
{
    return std::find(fAliveEntites.begin(), fAliveEntites.end(), entity) != fAliveEntites.end();
}

EStructProperty* EDataBase::AddComponent(Entity entity, const EValueDescription& description) 
{
    E_ASSERT_M(description.Valid(), "ERROR: Invalid value descrition!");
    E_ASSERT_M(description.GetType() == EValueType::STRUCT, "Component can only be inserted as struct");
    if (!IsAlive(entity)) { return nullptr; }

    if (!HasComponent(entity, description.GetId()))
    {
        EStructProperty* storage = static_cast<EStructProperty*>(EProperty::CreateFromDescription(description.GetId(), description));
        storage->SetChangeFunc([this, entity](EString ident){
            fEventDispatcher.Enqueue<ValueChangeEvent>({ident, entity});
        });

        fComponentStorage[description.GetId()][entity] = storage;

        fEventDispatcher.Enqueue<ComponentCreateEvent>({description.GetId(), entity});
        return storage;
    }
    return nullptr;
}

void EDataBase::RemoveComponent(Entity entity, const EValueDescription& componentId)
{
    E_ASSERT_M(componentId.Valid(), "ERROR: Invalid value descrition!");
    if (!IsAlive(entity)) { return; }
    if (!HasComponent(entity, componentId)) { return; }

    delete fComponentStorage[componentId.GetId()][entity];
    fComponentStorage[componentId.GetId()].erase(entity);

    fEventDispatcher.Enqueue<ComponentDeleteEvent>({componentId.GetId(), entity});
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

EStructProperty* EDataBase::GetComponent(Entity entity, const EValueDescription& componentId) 
{
    E_ASSERT_M(componentId.Valid(), "ERROR: Invalid value descrition!");
    return GetComponent(entity, componentId.GetId());
}

EStructProperty* EDataBase::GetComponent(Entity entity, const EValueDescription::t_ID& componentId) 
{
    if (!IsAlive(entity)) { return nullptr; }
    if (!HasComponent(entity, componentId)) { return nullptr; }

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

EProperty* EDataBase::GetValueByIdentifier(Entity entity, const EString& identifier) 
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
    EStructProperty* currentProp = GetComponent(entity, identList[0]);
    // if we couldnt find the component return
    if (!currentProp) { return nullptr; }
    if (identList.size() == 1) { return currentProp; }
    return currentProp->GetPropertyByIdentifier(identifier.substr(identList[0].length() + 1));
}

EVector<EStructProperty*> EDataBase::GetAllComponents(Entity entity) 
{
    EVector<EStructProperty*> result;
    for (auto& entry : fComponentStorage)
    {
        if (entry.second.find(entity) != entry.second.end())
        {
            result.push_back(entry.second[entity]);
        }
    }
    return result;
}

EUnorderedMap<EDataBase::Entity, EStructProperty*>& EDataBase::View(const EValueDescription& description) 
{
    return fComponentStorage[description.GetId()];
}