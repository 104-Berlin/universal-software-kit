#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;




ERegister::ERegister(const EString& name) 
    : fName(name)
{
    
}


EResourceManager& ERegister::GetResourceManager() 
{
    return fResourceManager;
}

ERegister::Entity ERegister::CreateEntity() 
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
    return newEntity;
}

void ERegister::DestroyEntity(Entity entity) 
{
    EVector<Entity>::iterator it = std::find(fAliveEntites.begin(), fAliveEntites.end(), entity);
    if (it == fAliveEntites.end())
    {
        return;
    }
    for (auto& entry : fComponentStorage)
    {
        delete entry.second[entity];
        entry.second.erase(entity);
    }
    fAliveEntites.erase(it);
    fDeadEntites.push_back(entity);
}

EVector<ERegister::Entity> ERegister::GetAllEntities() const
{
    return fAliveEntites;
}

void ERegister::Clear() 
{
    for (auto& entry : fComponentStorage)
    {
        for (auto& storage : entry.second)
        {
            delete storage.second;
        }
    }
    fComponentStorage.clear();
    fAliveEntites.clear();
    fDeadEntites.clear();
}

bool ERegister::IsAlive(Entity entity) 
{
    return std::find(fAliveEntites.begin(), fAliveEntites.end(), entity) != fAliveEntites.end();
}

void ERegister::InsertComponent(Entity entity, EValueDescription description) 
{
    E_ASSERT(description.Valid(), "ERROR: Invalid value descrition!");
    E_ASSERT(description.GetType() == EValueType::STRUCT, "Component can only be inserted as struct");
    if (!IsAlive(entity)) { return; }

    if (!HasComponent(entity, description))
    {
        EUnorderedMap<EString, EProperty*> properties;

        EStructProperty* storage = static_cast<EStructProperty*>(EProperty::CreateFromDescription(description.GetId(), description));
        
        fComponentStorage[description.GetId()][entity] = storage;
    }
}

void ERegister::RemoveComponent(Entity entity, EValueDescription componentId)
{
    E_ASSERT(componentId.Valid(), "ERROR: Invalid value descrition!");
    if (!IsAlive(entity)) { return; }
    if (!HasComponent(entity, componentId)) { return; }
    delete fComponentStorage[componentId.GetId()][entity];
    fComponentStorage[componentId.GetId()].erase(entity);
}

bool ERegister::HasComponent(Entity entity, EValueDescription componentId) 
{
    E_ASSERT(componentId.Valid(), "ERROR: Invalid value descrition!");
    return fComponentStorage[componentId.GetId()].find(entity) != fComponentStorage[componentId.GetId()].end();
}

EStructProperty* ERegister::GetComponent(Entity entity, EValueDescription componentId) 
{
    E_ASSERT(componentId.Valid(), "ERROR: Invalid value descrition!");
    if (!IsAlive(entity)) { return nullptr; }
    if (!HasComponent(entity, componentId)) { return nullptr; }

    return fComponentStorage[componentId.GetId()][entity];
}

EVector<EStructProperty*> ERegister::GetAllComponents(Entity entity) 
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

EUnorderedMap<ERegister::Entity, EStructProperty*>& ERegister::View(const EValueDescription& description) 
{
    return fComponentStorage[description.GetId()];
}