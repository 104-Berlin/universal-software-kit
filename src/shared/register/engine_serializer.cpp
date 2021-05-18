#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

EJson WriteStructToJs(EStructProperty* property);


EJson ESerializer::WriteSceneToJson(EScene* scene) 
{
    EJson result = EJson::object();

    EJson entityArray = EJson::array();
    for (EScene::Entity entity : scene->GetAllEntities())
    {
        EJson entityObject = EJson::object();
        for (EStructProperty* component : scene->GetAllComponents(entity))
        {
            entityObject[component->GetPropertyName()] = WriteStructToJs(component);
        }
        entityArray.push_back(entityObject);
    }
    result["Objects"] = entityArray;

    return result;
}


EJson WritePrimitiveToJs(EProperty* property)
{
    EString primitiveType = property->GetDescription()->GetId();

    if (primitiveType == E_TYPEID_BOOL)
    {
        return static_cast<EValueProperty<bool>*>(property)->GetValue();
    }
    else if (primitiveType == E_TYPEID_DOUBLE)
    {
        return static_cast<EValueProperty<double>*>(property)->GetValue();
    }
    else if (primitiveType == E_TYPEID_INTEGER)
    {
        return static_cast<EValueProperty<i32>*>(property)->GetValue();
    }
    else if (primitiveType == E_TYPEID_STRING)
    {
        return static_cast<EValueProperty<EString>*>(property)->GetValue();
    }
    return 0;
}

EJson WriteStructToJs(EStructProperty* property)
{
    EStructDescription* structDsc = static_cast<EStructDescription*>(property->GetDescription());
    EJson result = EJson::object();
    for (auto& entry : structDsc->GetFields())
    {
        result[entry.first] = ESerializer::WritePropertyToJs(property->GetProperty(entry.first));
    }
    return result;
}

EJson WriteEnumToJs(EEnumProperty* property)
{
    EJson result = EJson::object();

    result["CurrentValue"] = property->GetCurrentValue();

    return result;
}

EJson ESerializer::WritePropertyToJs(EProperty* property) 
{
    EValueDescription* description = property->GetDescription();
    EValueType type = description->GetType();

    switch (type)
    {
    case EValueType::PRIMITIVE: return WritePrimitiveToJs(property); break;
    case EValueType::STRUCT: return WriteStructToJs(static_cast<EStructProperty*>(property)); break;
    case EValueType::ENUM: return WriteEnumToJs(static_cast<EEnumProperty*>(property)); break;
    }
    return 0;
}
