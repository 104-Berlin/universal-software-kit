#include "engine.h"
#include "prefix_shared.h"


size_t Engine::GetComponentTypeSize(EComponentType type) 
{
    switch (type)
    {
    case EComponentType::INTEGER: return sizeof(i32);
    case EComponentType::DOUBLE: return sizeof(double);
    case EComponentType::BOOL: return sizeof(bool);
    case EComponentType::STRING: return sizeof(char*);
    case EComponentType::COMPONENT_REFERENCE: return sizeof(int);
    }
    return 0;
}
    
size_t Engine::GetComponentSize(EComponentDescription description) 
{
    size_t result = 0;
    for (auto entry : description.TypeDesciptions)
    {
        result += GetComponentTypeSize(entry.Type);
    }
    return result;
}
using namespace Engine;



EProperty::EProperty(const EString& name) 
    : fName(name)
{
    
}

const EString& EProperty::GetPropertyName() const
{
    return fName;
}


EScene::EScene(const EString& name) 
    : fName(name)
{
    
}

void EScene::RegisterComponent(EComponentDescription description) 
{
    E_ASSERT(fRegisteredComponents.find(description.ID) == fRegisteredComponents.end(), "Component allready registered!");
    fRegisteredComponents.insert({description.ID, description});
}

EResourceManager& EScene::GetResourceManager() 
{
    return fResourceManager;
}

EScene::Entity EScene::CreateEntity() 
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

void EScene::DestroyEntity(Entity entity) 
{
    EVector<Entity>::iterator it = std::find(fAliveEntites.begin(), fAliveEntites.end(), entity);
    if (it == fAliveEntites.end())
    {
        return;
    }
    for (auto& entry : fComponentStorage)
    {
        entry.second[entity].Reset();
        entry.second.erase(entity);
    }
    fAliveEntites.erase(it);
    fDeadEntites.push_back(entity);
}

bool EScene::IsAlive(Entity entity) 
{
    return std::find(fAliveEntites.begin(), fAliveEntites.end(), entity) != fAliveEntites.end();
}

void EScene::InsertComponent(Entity entity, EComponentDescription::ComponentID componentId) 
{
    if (!IsAlive(entity)) { return; }
    if (fRegisteredComponents.find(componentId) == fRegisteredComponents.end()) { return; }

    if (!HasComponent(entity, componentId))
    {
        EUnorderedMap<EString, EProperty*> properties;
        const EComponentDescription& componentDesc = fRegisteredComponents[componentId];
        for (const auto& entry : componentDesc.TypeDesciptions)
        {
            switch (entry.Type)
            {
            case EComponentType::INTEGER: properties.insert({entry.Name, new EValueProperty<i32>(entry.Name)}); break;
            case EComponentType::DOUBLE: properties.insert({entry.Name, new EValueProperty<double>(entry.Name)}); break;
            case EComponentType::STRING: properties.insert({entry.Name, new EValueProperty<EString>(entry.Name)}); break;
            case EComponentType::BOOL: properties.insert({entry.Name, new EValueProperty<bool>(entry.Name)}); break;
            case EComponentType::COMPONENT_REFERENCE: break;
            }
        }

        EComponentStorage storage(componentDesc, properties);
        fComponentStorage[componentId].insert({entity, storage});
    }
}

void EScene::RemoveComponent(Entity entity, EComponentDescription::ComponentID componentId) 
{
    if (!IsAlive(entity)) { return; }
    if (!HasComponent(entity, componentId)) { return; }
    fComponentStorage[componentId][entity].Reset();
    fComponentStorage[componentId].erase(entity);
}

bool EScene::HasComponent(Entity entity, EComponentDescription::ComponentID componentId) 
{
    return fComponentStorage[componentId].find(entity) != fComponentStorage[componentId].end();
}

EComponentStorage EScene::GetComponent(Entity entity, EComponentDescription::ComponentID componentId) 
{
    if (!IsAlive(entity)) { return EComponentStorage(); }
    if (!HasComponent(entity, componentId)) { return EComponentStorage(); }

    return fComponentStorage[componentId][entity];
}
