#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

EJson ESerializer::WriteStorageDescriptionToJson(const EValueDescription& description) 
{
    EJson result = EJson::object();

    result["Type"] = description.GetType();
    result["ID"] = description.GetId();
    switch (description.GetType())
    {
        case EValueType::ANY:
        case EValueType::PRIMITIVE:
        case EValueType::UNKNOWN: break;
        case EValueType::ARRAY:
        {
            result["ArrayType"] = WriteStorageDescriptionToJson(description.GetAsPrimitive());
            break;
        }
        case EValueType::STRUCT:
        {
            result["StructFields"] = EJson::array();
            EJson& structFieldJson = result["StructFields"];
            for (auto& entry : description.GetStructFields())
            {
                EJson fieldObject = EJson::object();
                fieldObject["PropertyName"] = entry.first;
                fieldObject["Description"] = WriteStorageDescriptionToJson(entry.second);
                structFieldJson.push_back(fieldObject);
            }
            break;
        }
        case EValueType::ENUM:
        {
            EJson& enumOptions = result["EnumOptions"];
            enumOptions = EJson::array();
            for (const EString& option : description.GetEnumOptions())
            {
                enumOptions.push_back(option.c_str());
            }
            break;
        }
    }

    return result;
}

EJson WriteStructToJs(EStructProperty* property);


EJson ESerializer::WriteSceneToJson(EDataBase* scene) 
{
    EUnorderedMap<EValueDescription::t_ID, EValueDescription> valueTypes;
    EJson result = EJson::object();

    EJson entityArray = EJson::array();
    for (EDataBase::Entity entity : scene->GetAllEntities())
    {
        EJson entityObject = EJson::object();
        for (ERef<EProperty> component : scene->GetAllComponents(entity))
        {
            EValueDescription dsc = component->GetDescription();
            if (valueTypes.find(dsc.GetId()) == valueTypes.end())
            {
                valueTypes[dsc.GetId()] = dsc;
            }
            entityObject[component->GetPropertyName()] = WritePropertyToJs(component.get());
        }
        entityArray.push_back(entityObject);
    }
    result["Objects"] = entityArray;


    EJson valueTypeArray = EJson::array();
    for (auto& entry : valueTypes)
    {
        valueTypeArray.push_back(WriteStorageDescriptionToJson(entry.second));
    }

    result["ValueTypes"] = valueTypeArray;

    return result;
}


EJson WritePrimitiveToJs(EProperty* property)
{
    EString primitiveType = property->GetDescription().GetId();

    if (primitiveType == E_TYPEID_BOOL)
    {
        return static_cast<EValueProperty<bool>*>(property)->GetValue();
    }
    else if (primitiveType == E_TYPEID_DOUBLE)
    {
        return static_cast<EValueProperty<double>*>(property)->GetValue();
    }
    else if (primitiveType == E_TYPEID_FLOAT)
    {
        return static_cast<EValueProperty<float>*>(property)->GetValue();
    }
    else if (primitiveType == E_TYPEID_INTEGER)
    {
        return static_cast<EValueProperty<i32>*>(property)->GetValue();
    }
    else if (primitiveType == E_TYPEID_UNSIGNED_INTEGER)
    {
        return static_cast<EValueProperty<u32>*>(property)->GetValue();
    }
    else if (primitiveType == E_TYPEID_UNSIGNED_BIG_INTEGER)
    {
        return static_cast<EValueProperty<u64>*>(property)->GetValue();
    }
    else if (primitiveType == E_TYPEID_STRING)
    {
        return static_cast<EValueProperty<EString>*>(property)->GetValue();
    }
    return 0;
}

EJson WriteStructToJs(EStructProperty* property)
{
    EValueDescription structDsc = property->GetDescription();
    EJson result = EJson::object();
    for (auto& entry : structDsc.GetStructFields())
    {
        result[entry.first] = ESerializer::WritePropertyToJs(property->GetProperty(entry.first).get());
    }
    return result;
}

EJson WriteAnyToJs(EStructProperty* property)
{
    ERef<EProperty> value = property->GetProperty("value");
    if (!value)
    {
        return EJson();
    }
    return ESerializer::WritePropertyToJs(value.get(), true);
}

EJson WriteEnumToJs(EEnumProperty* property)
{
    EJson result = EJson::object();

    result["CurrentValue"] = property->GetCurrentValue();

    return result;
}

EJson WriteArrayToJs(EArrayProperty* property)
{
    EJson result = EJson::array();
    for (ERef<EProperty> property : property->GetElements())
    {
        result.push_back(ESerializer::WritePropertyToJs(property.get()));
    }
    return result;
}

EJson ESerializer::WritePropertyToJs(EProperty* property, bool writeDescription) 
{
    EValueDescription description = property->GetDescription();
    EValueType type = description.GetType();
    EJson valueJson = EJson::object();
    switch (type)
    {
    case EValueType::PRIMITIVE: valueJson = WritePrimitiveToJs(property); break;
    case EValueType::ARRAY: valueJson = WriteArrayToJs(static_cast<EArrayProperty*>(property)); break;
    case EValueType::ANY: valueJson = WriteAnyToJs(static_cast<EStructProperty*>(property)); break;
    case EValueType::STRUCT: valueJson = WriteStructToJs(static_cast<EStructProperty*>(property)); break;
    case EValueType::ENUM: valueJson = WriteEnumToJs(static_cast<EEnumProperty*>(property)); break;
    case EValueType::UNKNOWN: return 0;
    }

    if (!writeDescription)
    {
        return valueJson;
    }
    else
    {
        EJson result = EJson::object();
        result["Value"] = valueJson;
        result["ValueDescription"] = WriteStorageDescriptionToJson(property->GetDescription());
        return result;
    }

    return 0;
}

ESharedBuffer ESerializer::WriteFullSceneBuffer(EDataBase* reg) 
{
    EFileCollection fileCollection;

    EString jsonString = WriteSceneToJson(reg).dump();
    ESharedBuffer jsonBuffer;
    jsonBuffer.InitWith<char>(jsonString.size() + 1);
    char* data = jsonBuffer.Data<char>();
    strcpy(data, jsonString.c_str());


    fileCollection.AddFile("CORE/scene.json", jsonBuffer);

    for (EResource* resourceData : reg->GetResourceManager().GetAllResource())
    {
        EFile file(resourceData->GetTempFilePath());
        if (!file.Exist()) { continue; }
        file.LoadToMemory();
        ESharedBuffer resourceBuffer = file.GetBuffer();

        fileCollection.AddFile(std::to_string(resourceData->GetID()), resourceBuffer);
    }

    
    return fileCollection.GetCompleteBuffer();
}

EJson ESerializer::WritEResourceBaseToJson(EResource* data, bool writeData) 
{
    EJson result = EJson::object();

    result["ID"] = data->GetID();
    result["Type"] = data->GetResourceType();
    result["Name"] = data->GetName();

    if (writeData && data)
    {
        ESharedBuffer resourceData = data->GetBuffer();
        if (!resourceData.IsNull())
        {
            result["Data"] = Base64::Encode(resourceData.Data<u8>(), resourceData.GetSizeInByte());
        }
    }

    return result;
}
