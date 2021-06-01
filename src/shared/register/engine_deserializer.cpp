#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

void EDeserializer::ReadSceneFromJson(const EJson& json, EScene* saveToScene, const EVector<EValueDescription*>& registeredTypes) 
{
    auto findType = [&registeredTypes](const EString& id) -> EValueDescription* {
        for (EValueDescription* dsc : registeredTypes)
        {
            if (dsc->GetId() == id)
            {
                return dsc;
            }
        }
        return nullptr;
    };

    saveToScene->Clear();
    
    if (!json["Objects"].is_null())
    {
        for (const EJson& entityObject : json["Objects"])
        {
            EScene::Entity entity = saveToScene->CreateEntity();
            for (const auto& it : entityObject.items())
            {
                EString id = it.key();
                EValueDescription* description = findType(id);
                if (description)
                {
                    saveToScene->InsertComponent(entity, description);
                    EStructProperty* component = saveToScene->GetComponent(entity, description);
                    ReadPropertyFromJson(entityObject[component->GetPropertyName()], component);
                }
            }
        }
    }
}

void ReadPrimitiveFromJson(const EJson& json, EProperty* property)
{
    EString primitiveType = property->GetDescription()->GetId();

    if (primitiveType == E_TYPEID_BOOL)
    {
        if (json.is_boolean())
        {
            static_cast<EValueProperty<bool>*>(property)->SetValue(json.get<bool>());
        }
    }
    else if (primitiveType == E_TYPEID_DOUBLE)
    {
        if (json.is_number_float())
        {
            static_cast<EValueProperty<double>*>(property)->SetValue(json.get<double>());
        }
    }
    else if (primitiveType == E_TYPEID_INTEGER)
    {
        if (json.is_number_integer())
        {
            static_cast<EValueProperty<i32>*>(property)->SetValue(json.get<i32>());
        }
    }
    else if (primitiveType == E_TYPEID_STRING)
    {
        if (json.is_string())
        {
            static_cast<EValueProperty<EString>*>(property)->SetValue(json.get<EString>());
        }
    }
}


void ReadEnumFromJson(const EJson& json, EEnumProperty* property)
{
    if (json["CurrentValue"].is_string())
    {
        property->SetCurrentValue(json["CurrentValue"].get<EString>());
    }
}

void ReadArrayFromJson(const EJson& json, EArrayProperty* property)
{
    property->Clear();
    for (const EJson& entry : json)
    {
        EProperty* newElement = property->AddElement();
        EDeserializer::ReadPropertyFromJson(entry, newElement);
    }
}

void ReadStructFromJson(const EJson& json, EStructProperty* property)
{
    EStructDescription* description = static_cast<EStructDescription*>(property->GetDescription());
    
    for (auto& entry : description->GetFields())
    {
        EValueType fieldType = entry.second->GetType();

        EDeserializer::ReadPropertyFromJson(json[entry.first], property->GetProperty(entry.first));
    }
}

void EDeserializer::ReadPropertyFromJson(const EJson& json, EProperty* property) 
{
    EValueDescription* currentDsc = property->GetDescription();
    EValueType currentType = currentDsc->GetType();
    switch (currentType)
    {
    case EValueType::PRIMITIVE: ReadPrimitiveFromJson(json, property); break;
    case EValueType::STRUCT: ReadStructFromJson(json, static_cast<EStructProperty*>(property)); break;
    case EValueType::ENUM: ReadEnumFromJson(json, static_cast<EEnumProperty*>(property)); break;
    case EValueType::ARRAY: ReadArrayFromJson(json, static_cast<EArrayProperty*>(property)); break;
    }
}