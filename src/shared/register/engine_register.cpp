#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;




ERegister::ERegister(const EString& name) 
    : fName(name)
{
    
}

Engine::ERegister::~ERegister() 
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
        fEventDispatcher.Enqueue<ComponentDeleteEvent>({entry.first, entity});
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
            fEventDispatcher.Enqueue<ComponentDeleteEvent>({entry.first, storage.first});
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

EStructProperty* ERegister::AddComponent(Entity entity, const EValueDescription& description) 
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

void ERegister::RemoveComponent(Entity entity, const EValueDescription& componentId)
{
    E_ASSERT_M(componentId.Valid(), "ERROR: Invalid value descrition!");
    if (!IsAlive(entity)) { return; }
    if (!HasComponent(entity, componentId)) { return; }

    delete fComponentStorage[componentId.GetId()][entity];
    fComponentStorage[componentId.GetId()].erase(entity);

    fEventDispatcher.Enqueue<ComponentDeleteEvent>({componentId.GetId(), entity});
}

bool ERegister::HasComponent(Entity entity, const EValueDescription& componentId) 
{
    E_ASSERT_M(componentId.Valid(), "ERROR: Invalid value descrition!");
    return HasComponent(entity, componentId.GetId());
}

bool Engine::ERegister::HasComponent(Entity entity, const EValueDescription::t_ID& componentId) 
{
    return fComponentStorage[componentId].find(entity) != fComponentStorage[componentId].end();
}

EStructProperty* ERegister::GetComponent(Entity entity, const EValueDescription& componentId) 
{
    E_ASSERT_M(componentId.Valid(), "ERROR: Invalid value descrition!");
    return GetComponent(entity, componentId.GetId());
}

EStructProperty* ERegister::GetComponent(Entity entity, const EValueDescription::t_ID& componentId) 
{
    if (!IsAlive(entity)) { return nullptr; }
    if (!HasComponent(entity, componentId)) { return nullptr; }

    return fComponentStorage[componentId][entity];
}

void Engine::ERegister::DisconnectEvents() 
{
    fEventDispatcher.DisconnectEvents();
}

void Engine::ERegister::UpdateEvents() 
{
    fEventDispatcher.Update();
}

EProperty* ERegister::GetValueByIdentifier(Entity entity, const EString& identifier) 
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
    EProperty* currentProp = static_cast<EProperty*>(GetComponent(entity, identList[0]));
    // if we couldnt find the component return
    if (!currentProp) { return nullptr; }

    for (size_t i = 1; i < identList.size(); i++)
    {
        if (!currentProp) { return nullptr; }
        const EString& currentIdent = identList[i];
        EValueDescription currentDsc = currentProp->GetDescription();

        if (currentDsc.IsArray())
        {
            if (std::regex_match(currentIdent, std::regex("[0-9]+", std::regex::ECMAScript)))
            {
                size_t arrayIndex = std::atoi(currentIdent.c_str());
                currentProp = static_cast<EArrayProperty*>(currentProp)->GetElement(arrayIndex);
            }
            else
            {
                currentProp = nullptr;
            }
        }
        else
        {
            switch (currentDsc.GetType())
            {
            case EValueType::PRIMITIVE:
            {
                break;
            }
            case EValueType::ARRAY:
            {

                break;
            }
            case EValueType::STRUCT:
            {
                EStructProperty* structProp = static_cast<EStructProperty*>(currentProp);
                currentProp = structProp->GetProperty(currentIdent);
                break;
            }
            case EValueType::ENUM:
            {
                break;
            }
            case EValueType::UNKNOWN:
            {
                break;
            }
            }
        }
    }
    return currentProp;
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