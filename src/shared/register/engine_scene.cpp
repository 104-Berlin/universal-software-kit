#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;



EProperty::EProperty(const EString& name, EValueDescription* description) 
    : fName(name), fDescription(description)
{
    
}

const EString& EProperty::GetPropertyName() const
{
    return fName;
}

EValueDescription* EProperty::GetDescription() const
{
    return fDescription;
}



EProperty* EProperty::CreateFromDescription(const EString& name, EValueDescription* description) 
{
    EValueType type = description->GetType();
    switch (type)
    {
    case EValueType::PRIMITIVE: return CreatePropertyPrimitive(name, description);
    case EValueType::STRUCT: return CreatePropertyStruct(name, static_cast<EStructDescription*>(description));
    case EValueType::ENUM: return CreatePropertyEnum(name, static_cast<EEnumDescription*>(description));
    case EValueType::ARRAY: return CreatePropertyArray(name, static_cast<EArrayDescription*>(description));
    }
    return nullptr;
}

EProperty* EProperty::CreatePropertyStruct(const EString& name, EStructDescription* description) 
{
    EVector<EProperty*> fields;
    for (auto& entry : description->GetFields())
    {
        EProperty* newField = CreateFromDescription(entry.first, entry.second);
        if (newField)
        {
            fields.push_back(newField);
        }
    }
    return new EStructProperty(name, description, fields);
}

EProperty* EProperty::CreatePropertyPrimitive(const EString& name, EValueDescription* description) 
{
    const EString& primitiveId = description->GetId();
    if (primitiveId == E_TYPEID_STRING) { return new EValueProperty<EString>(name, description); } 
    else if (primitiveId == E_TYPEID_INTEGER) { return new EValueProperty<i32>(name, description); }
    else if (primitiveId == E_TYPEID_DOUBLE) { return new EValueProperty<double>(name, description); }
    else if (primitiveId == E_TYPEID_BOOL) { return new EValueProperty<bool>(name, description); }
    return nullptr;
}

EProperty* EProperty::CreatePropertyEnum(const EString& name, EEnumDescription* descrption) 
{
    EEnumProperty* result = new EEnumProperty(name, descrption);
    return result;
}

EProperty* EProperty::CreatePropertyArray(const EString& name, EArrayDescription* description) 
{
    EArrayProperty* result = new EArrayProperty(name, description);
    return result;
}


EStructProperty::EStructProperty(const EString& name, EStructDescription* description, const EVector<EProperty*>& properties) 
    : EProperty(name, description), fProperties(properties)
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

EEnumProperty::EEnumProperty(const EString& name, EEnumDescription* description, const EString& initValue) 
    : EProperty(name, description), fValue(initValue)
{
    const EVector<EString>& options = description->GetOptions();
    if (options.size() > 0)
    {
        if (fValue.empty())
        {
            fValue = options[0];
        }
        else
        {
            EVector<EString>::const_iterator foundOption = std::find(options.begin(), options.end(), fValue);
            if (foundOption == options.end())
            {
                fValue = options[0];
            }
        }
    }
    else
    {
        fValue = "";
    }
}

EEnumProperty::~EEnumProperty() 
{
    
}

void EEnumProperty::SetCurrentValue(const EString& value) 
{
    fValue = value;
}

const EString& EEnumProperty::GetCurrentValue() const
{
    return fValue;
}

EArrayProperty::EArrayProperty(const EString& name, EArrayDescription* description) 
    : EProperty(name, description)
{
    
}

EArrayProperty::~EArrayProperty() 
{
    Clear();
}

EProperty* EArrayProperty::AddElement() 
{
    EArrayDescription* arrayDsc = static_cast<EArrayDescription*>(fDescription);
    EString elementName = std::to_string(fElements.size());
    EProperty* result = EProperty::CreateFromDescription(elementName, arrayDsc->GetElementType());
    fElements.push_back(result);
    return result;
}

EProperty* EArrayProperty::GetElement(size_t index) 
{
    if (index >= fElements.size()) 
    {
        E_ERROR("ERROR: Index out of bounds exception! Cant access element at index " + std::to_string(index) + " for array \"" + GetPropertyName() + "\"");
        return nullptr;
    }
    return fElements[index];
}

void EArrayProperty::RemoveElement(size_t index) 
{
    if (index >= fElements.size()) 
    {
        E_ERROR("ERROR: Index out of bounds exception! Cant delete element at index " + std::to_string(index) + " for array \"" + GetPropertyName() + "\"");
        return;
    }
    delete fElements[index];
    fElements.erase(fElements.begin() + index);
}

const EVector<EProperty*>& EArrayProperty::GetElements() const
{
    return fElements;
}

void EArrayProperty::Clear() 
{
    for (EProperty* property : fElements)
    {
        delete property;
    }
    fElements.clear();
}


EScene::EScene(const EString& name) 
    : fName(name)
{
    
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
        delete entry.second[entity];
        entry.second.erase(entity);
    }
    fAliveEntites.erase(it);
    fDeadEntites.push_back(entity);
}

EVector<EScene::Entity> EScene::GetAllEntities() const
{
    return fAliveEntites;
}

void EScene::Clear() 
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

bool EScene::IsAlive(Entity entity) 
{
    return std::find(fAliveEntites.begin(), fAliveEntites.end(), entity) != fAliveEntites.end();
}

void EScene::InsertComponent(Entity entity, EValueDescription* description) 
{
    E_ASSERT(description, "ERROR: Invalid value descrition!");
    if (!IsAlive(entity)) { return; }
    E_ASSERT(description->GetType() == EValueType::STRUCT, "Component can only be inserted as struct");

    if (!HasComponent(entity, description))
    {
        EUnorderedMap<EString, EProperty*> properties;

        EStructDescription* structDescription = static_cast<EStructDescription*>(description);

        EStructProperty* storage = static_cast<EStructProperty*>(EProperty::CreateFromDescription(description->GetId(), structDescription));


        fComponentStorage[description][entity] = storage;
    }
}

void EScene::RemoveComponent(Entity entity, EValueDescription* componentId) 
{
    E_ASSERT(componentId, "ERROR: Invalid value descrition!");
    if (!IsAlive(entity)) { return; }
    if (!HasComponent(entity, componentId)) { return; }
    delete fComponentStorage[componentId][entity];
    fComponentStorage[componentId].erase(entity);
}

bool EScene::HasComponent(Entity entity, EValueDescription* componentId) 
{
    E_ASSERT(componentId, "ERROR: Invalid value descrition!");
    return fComponentStorage[componentId][entity];
}

EStructProperty* EScene::GetComponent(Entity entity, EValueDescription* componentId) 
{
    E_ASSERT(componentId, "ERROR: Invalid value descrition!");
    if (!IsAlive(entity)) { return nullptr; }
    if (!HasComponent(entity, componentId)) { return nullptr; }

    return fComponentStorage[componentId][entity];
}

EVector<EStructProperty*> EScene::GetAllComponents(Entity entity) 
{
    EVector<EStructProperty*> result;
    for (auto& entry : fComponentStorage)
    {
        if (entry.second[entity])
        {
            result.push_back(entry.second[entity]);
        }
    }
    return result;
}