#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;



EProperty::EProperty(const EString& name) 
    : fName(name)
{
    
}

const EString& EProperty::GetPropertyName() const
{
    return fName;
}

EStructProperty::EStructProperty(const EString& name, const EVector<EProperty*>& properties) 
    : EProperty(name), fProperties(properties)
{
    
}

EStructProperty::~EStructProperty() 
{
    for (EProperty* property : fProperties)
    {
        delete property;
    }
    fProperties.clear();
}

bool EStructProperty::HasProperty(const EString& propertyName) const
{
    for (EProperty* prop : fProperties)
    {
        if (prop->GetPropertyName() == propertyName)
        {
            return true;
        }
    }
    return false;
}

EProperty* EStructProperty::GetProperty(const EString& propertyName) 
{
    for (EProperty* property : fProperties)
    {
        if (property->GetPropertyName() == propertyName)
        {
            return property;
        }
    }
    return nullptr;
}

const EProperty* EStructProperty::GetProperty(const EString& propertyName) const
{
    for (const EProperty* property : fProperties)
    {
        if (property->GetPropertyName() == propertyName)
        {
            return property;
        }
    }
    return nullptr;
}

EComponentStorage::EComponentStorage(EComponentDescription dsc, const EUnorderedMap<EString, EProperty*>& propInit) 
    : fDsc(dsc), fProperties(propInit)
{

}

EComponentStorage::~EComponentStorage() 
{
    fProperties.clear();
}

EComponentStorage::operator bool() const
{
    return Valid();
}

bool EComponentStorage::Valid() const
{
    return fDsc.Valid();
}

void EComponentStorage::Reset() 
{
    for (auto& property : fProperties)
    {
        delete property.second;
    }
}

bool EComponentStorage::HasProperty(const EString& propertyName) 
{
    return fProperties.find(propertyName) != fProperties.end();
}

bool EComponentStorage::GetProperty(const EString& propertyName, EValueProperty<i32>** outValue) 
{
    if (!HasProperty(propertyName))
    {
        return false;
    }
    EValueTypeDescription typeDsc;
    if (!fDsc.GetTypeDescription(propertyName, &typeDsc))
    {
        E_WARN("Proeprty with name not added. Reset the component!");
        return false;
    }
    if (typeDsc.Type != EValueType::INTEGER)
    {
        E_ERROR("Wrong Type getting property " + propertyName);
        return false;
    }
    *outValue = (EValueProperty<i32>*) fProperties[propertyName];
    return true;
}

bool EComponentStorage::GetProperty(const EString& propertyName, EValueProperty<double>** outValue) 
{
    if (!HasProperty(propertyName))
    {
        return false;
    }
    EValueTypeDescription typeDsc;
    if (!fDsc.GetTypeDescription(propertyName, &typeDsc))
    {
        E_WARN("Proeprty with name not added. Reset the component!");
        return false;
    }
    if (typeDsc.Type != EValueType::DOUBLE)
    {
        E_ERROR("Wrong Type getting property " + propertyName);
        return false;
    }
    *outValue = (EValueProperty<double>*) fProperties[propertyName];
    return true;
}

bool EComponentStorage::GetProperty(const EString& propertyName, EValueProperty<bool>** outValue) 
{
    if (!HasProperty(propertyName))
    {
        return false;
    }
    EValueTypeDescription typeDsc;
    if (!fDsc.GetTypeDescription(propertyName, &typeDsc))
    {
        E_WARN("Proeprty with name not added. Reset the component!");
        return false;
    }
    if (typeDsc.Type != EValueType::BOOL)
    {
        E_ERROR("Wrong Type getting property " + propertyName);
        return false;
    }
    *outValue = (EValueProperty<bool>*) fProperties[propertyName];
    return true;
}

bool EComponentStorage::GetProperty(const EString& propertyName, EValueProperty<EString>** outValue) 
{
    if (!HasProperty(propertyName))
    {
        return false;
    }
    EValueTypeDescription typeDsc;
    if (!fDsc.GetTypeDescription(propertyName, &typeDsc))
    {
        E_WARN("Proeprty with name not added. Reset the component!");
        return false;
    }
    if (typeDsc.Type != EValueType::STRING)
    {
        E_ERROR("Wrong Type getting property " + propertyName);
        return false;
    }
    *outValue = (EValueProperty<EString>*) fProperties[propertyName];
    return true;
}

bool EComponentStorage::GetProperty(const EString& propertyName, EStructProperty** outValue) 
{
    if (!HasProperty(propertyName))
    {
        return false;
    }
    EStructTypeDescription typeDsc;
    if (!fDsc.GetStructDescription(propertyName, &typeDsc))
    {
        E_WARN("Proeprty with name not added. Reset the component!");
        return false;
    }
    *outValue = (EStructProperty*) fProperties[propertyName];
    return true;
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
        for (const auto& entry : componentDesc.ValueTypeDesciptions)
        {
            switch (entry.Type)
            {
            case EValueType::INTEGER: properties.insert({entry.Name, new EValueProperty<i32>(entry.Name)}); break;
            case EValueType::DOUBLE: properties.insert({entry.Name, new EValueProperty<double>(entry.Name)}); break;
            case EValueType::STRING: properties.insert({entry.Name, new EValueProperty<EString>(entry.Name)}); break;
            case EValueType::BOOL: properties.insert({entry.Name, new EValueProperty<bool>(entry.Name)}); break;
            case EValueType::COMPONENT_REFERENCE: break;
            }
        }

        for (const auto& entry : componentDesc.StructTypeDescriptions)
        {
            EVector<EProperty*> structProps;
            for (const auto& valueType : entry.Fields)
            {
                switch (valueType.Type)
                {
                case EValueType::INTEGER: structProps.push_back(new EValueProperty<i32>(valueType.Name)); break;
                case EValueType::DOUBLE: structProps.push_back(new EValueProperty<double>(valueType.Name)); break;
                case EValueType::STRING: structProps.push_back(new EValueProperty<EString>(valueType.Name)); break;
                case EValueType::BOOL: structProps.push_back(new EValueProperty<bool>(valueType.Name)); break;
                case EValueType::COMPONENT_REFERENCE: break;
                }
            }
            EStructProperty* newProperty = new EStructProperty(entry.Name, structProps);
            properties.insert({entry.Name, newProperty});
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
