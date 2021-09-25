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
        case EValueType::PRIMITIVE:
        case EValueType::UNKNOWN: break;
        case EValueType::ARRAY:
        {
            result["ArrayType"] = WriteStorageDescriptionToJson(description.GetAsPrimitive());
            break;
        }
        case EValueType::STRUCT:
        {
            result["StructFields"] = EJson::object();
            EJson& structFieldJson = result["StructFields"];
            for (auto& entry : description.GetStructFields())
            {
                structFieldJson[entry.first.c_str()] = WriteStorageDescriptionToJson(*entry.second);
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


EJson ESerializer::WriteSceneToJson(ERegister* scene) 
{
    EJson result = EJson::object();

    EJson entityArray = EJson::array();
    for (ERegister::Entity entity : scene->GetAllEntities())
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
    EString primitiveType = property->GetDescription().GetId();

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

EJson WriteArrayToJs(EArrayProperty* property)
{
    EJson result = EJson::array();
    for (EProperty* property : property->GetElements())
    {
        result.push_back(ESerializer::WritePropertyToJs(property));
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

ESharedBuffer ESerializer::WriteFullSceneBuffer(ERegister* reg) 
{
    EFileCollection fileCollection;

    EString jsonString = WriteSceneToJson(reg).dump();
    ESharedBuffer jsonBuffer;
    jsonBuffer.InitWith<char>(jsonString.size() + 1);
    char* data = jsonBuffer.Data<char>();
    strcpy(data, jsonString.c_str());


    fileCollection.AddFile("CORE/scene.json", jsonBuffer);

    for (EResourceData* resourceData : reg->GetResourceManager().GetAllResource())
    {
        size_t bufferSize = resourceData->Type.length() + 1 + sizeof(EResourceData::t_ID) + sizeof(u64) + resourceData->DataSize + sizeof(u64) + resourceData->UserDataSize;
        ESharedBuffer resourceBuffer;
        resourceBuffer.InitWith<u8>(bufferSize);

        u8* pointer = resourceBuffer.Data<u8>();

        strcpy((char*) pointer, resourceData->Type.c_str());
        pointer += resourceData->Type.size() + 1;

        EFileCollection::WriteU64(pointer, resourceData->ID);
        pointer += sizeof(EResourceData::t_ID);

        EFileCollection::WriteU64(pointer, resourceData->DataSize);
        pointer += sizeof(u64);

        memcpy(pointer, resourceData->Data, resourceData->DataSize);
        pointer += resourceData->DataSize;

        if (!resourceData->UserData)
        {
            EFileCollection::WriteU64(pointer, 0);
        }
        else
        {
            EFileCollection::WriteU64(pointer, resourceData->UserDataSize);
            pointer += sizeof(u64);
            
            memcpy(pointer, resourceData->UserData, resourceData->UserDataSize);
        }
        EString fileName;
        if (!resourceData->PathToFile.empty())
        {
            fileName = resourceData->PathToFile;
        }
        else 
        {
            fileName = resourceData->Type + resourceData->Name;
        }
        fileCollection.AddFile(fileName, resourceBuffer);

    }

    
    return fileCollection.GetCompleteBuffer();
}
