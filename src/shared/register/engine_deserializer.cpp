#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

bool EDeserializer::ReadStorageDescriptionFromJson(const EJson& json, EValueDescription* description) 
{
    if (json.is_null()) { return false; }
    if (!json["Type"].is_null() && !json["ID"].is_null())
    {
        if (!json["Type"].is_number_integer()) { return false; }
        if (!json["ID"].is_string()) { return false; }

        EString id = json["ID"].get<EString>();
        EValueType type = (EValueType) json["Type"].get<int>();

        EValueDescription newValueType = EValueDescription(type, id);
        switch (type)
        {
        case EValueType::ANY:
        case EValueType::PRIMITIVE:
        case EValueType::UNKNOWN: break;
        case EValueType::ARRAY:
        {
            const EJson& arrayType = json["ArrayType"];
            if (!arrayType.is_object() || arrayType.size() == 0) { return false; }
            EValueDescription arrayTypeDsc;
            if (!ReadStorageDescriptionFromJson(arrayType, &arrayTypeDsc)) { return false; }
            newValueType.SetArrayType(arrayTypeDsc);
            break;
        }
        case EValueType::STRUCT:
        {
            const EJson& structFieldsJson = json["StructFields"];
            if (!structFieldsJson.is_array() || structFieldsJson.size() == 0) { return false; }
            for (const auto& entry : structFieldsJson)
            {
                if (entry.is_object() && entry["PropertyName"].is_string() && entry["Description"].is_object())
                {
                    EValueDescription fieldDesc;
                    if (!ReadStorageDescriptionFromJson(entry["Description"], &fieldDesc)) { return false; }
                    EString propertyName = entry["PropertyName"].get<EString>();
                    newValueType.AddStructField(propertyName, fieldDesc);
                }
            }
            break;
        }
        case EValueType::ENUM:
        {
            const EJson& enumOptionJson = json["EnumOptions"];
            if (!enumOptionJson.is_array()) { return false; }
            for (auto& entry : enumOptionJson)
            {
                if (!entry.is_string()) { return false; }
                newValueType.AddEnumOption(entry.get<EString>());
            }
            break;
        }
        }
        *description = newValueType;
    }
    return true;
}

bool EDeserializer::ReadSceneFromJson(const EJson& json, EDataBase* saveToScene) 
{
    saveToScene->Clear();
    if (!json["ValueTypes"].is_array()) { E_ERROR("Reading Register from json. No found storage types!"); return false; }

    EUnorderedMap<EValueDescription::t_ID, EValueDescription> registeredTypes;
    for (auto& entry : json["ValueTypes"])
    {
        EValueDescription dsc;
        if (!ReadStorageDescriptionFromJson(entry, &dsc))
        {
            E_ERROR("Reading scene. Json has wrong format!");
            return false;
        }
        registeredTypes[dsc.GetId()] = dsc;
    }

    
    if (!json["Objects"].is_null())
    {
        for (const EJson& entityObject : json["Objects"])
        {
            EDataBase::Entity entity = saveToScene->CreateEntity();
            for (const auto& it : entityObject.items())
            {
                EString id = it.key();
                EValueDescription description = registeredTypes[id];
                if (description.Valid())
                {
                    saveToScene->AddComponent(entity, description);
                    EStructProperty* component = saveToScene->GetComponent(entity, description);
                    ReadPropertyFromJson(entityObject[component->GetPropertyName()], component);
                }
            }
        }
    }
    return true;
}

bool ReadPrimitiveFromJson(const EJson& json, EProperty* property)
{
    EString primitiveType = property->GetDescription().GetId();

    if (primitiveType == E_TYPEID_BOOL)
    {
        if (json.is_boolean())
        {
            static_cast<EValueProperty<bool>*>(property)->SetValue(json.get<bool>());
            return true;
        }
    }
    else if (primitiveType == E_TYPEID_DOUBLE)
    {
        if (json.is_number_float())
        {
            static_cast<EValueProperty<double>*>(property)->SetValue(json.get<double>());
            return true;
        }
    }
    else if (primitiveType == E_TYPEID_FLOAT)
    {
        if (json.is_number_float())
        {
            static_cast<EValueProperty<float>*>(property)->SetValue(json.get<float>());
            return true;
        }
    }
    else if (primitiveType == E_TYPEID_INTEGER)
    {
        if (json.is_number_integer())
        {
            static_cast<EValueProperty<i32>*>(property)->SetValue(json.get<i32>());
            return true;
        }
    }
    else if (primitiveType == E_TYPEID_UNSIGNED_INTEGER)
    {
        if (json.is_number_integer())
        {
            static_cast<EValueProperty<u32>*>(property)->SetValue(json.get<u32>());
            return true;
        }
    }
    else if (primitiveType == E_TYPEID_UNSIGNED_BIG_INTEGER)
    {
        if (json.is_number_integer())
        {
            static_cast<EValueProperty<u64>*>(property)->SetValue(json.get<u64>());
            return true;
        }
    }
    else if (primitiveType == E_TYPEID_STRING)
    {
        if (json.is_string())
        {
            static_cast<EValueProperty<EString>*>(property)->SetValue(json.get<EString>());
            return true;
        }
    }
    return false;
}


bool ReadEnumFromJson(const EJson& json, EEnumProperty* property)
{
    if (json["CurrentValue"].is_string())
    {
        property->SetCurrentValue(json["CurrentValue"].get<EString>());
        return true;
    }
    else if (json["CurrentValue"].is_number_integer())
    {
        property->SetCurrentValue(json["CurrentValue"].get<u32>());
        return true;
    }
    return false;
}

bool ReadArrayFromJson(const EJson& json, EArrayProperty* property)
{
    property->Clear();
    for (const EJson& entry : json)
    {
        EProperty* newElement = property->AddElement();
        EDeserializer::ReadPropertyFromJson(entry, newElement);
    }
    return true;
}

bool ReadStructFromJson(const EJson& json, EStructProperty* property)
{
    EValueDescription description = property->GetDescription();
    
    for (auto& entry : description.GetStructFields())
    {
        EValueType fieldType = entry.second.GetType();

        EDeserializer::ReadPropertyFromJson(json[entry.first], property->GetProperty(entry.first));
    }
    return true;
}

bool EDeserializer::ReadPropertyFromJson(const EJson& json, EProperty* property) 
{
    EValueDescription currentDsc = property->GetDescription();
    EValueType currentType = currentDsc.GetType();

    switch (currentType)
    {
    case EValueType::ANY: 
    case EValueType::PRIMITIVE: ReadPrimitiveFromJson(json, property); break;
    case EValueType::ARRAY: ReadArrayFromJson(json, static_cast<EArrayProperty*>(property)); break;
    case EValueType::STRUCT: ReadStructFromJson(json, static_cast<EStructProperty*>(property)); break;
    case EValueType::ENUM: ReadEnumFromJson(json, static_cast<EEnumProperty*>(property)); break;
    case EValueType::UNKNOWN: break;
    }
    return true;
}

bool EDeserializer::ReadPropertyFromJson_WithDescription(const EJson& json, EProperty** property) 
{
    EValueDescription dsc;
    if (json["ValueDescription"].is_object() && ReadStorageDescriptionFromJson(json["ValueDescription"], &dsc))
    {
        *property = EProperty::CreateFromDescription(dsc.GetId(), dsc);
        if (json["Value"].is_object() && ReadPropertyFromJson(json["Value"], *property))
        {
            return true;
        }
        delete *property;
        return false;
    }
    return false;
}

bool EDeserializer::ReadResourceFromJson(const EJson& json, EResourceData* resData, bool withData) 
{
    if (!json.is_object()) { return false; }
    if (json.size() == 0)  { return false;}
    if (json["ID"].is_number_integer() && json["Type"].is_string() && json["Name"].is_string() && json["PathToFile"].is_string())
    {
        resData->ID = json["ID"].get<EResourceData::t_ID>();
        resData->Type = json["Type"].get<EString>();
        resData->Name = json["Name"].get<EString>();
        resData->PathToFile = json["PathToFile"].get<EString>();
        if (withData && json["Data"].is_string() && json["UserData"].is_string())
        {
            u8* data;
            size_t dataLen;
            u8* userData;
            size_t userDataLen;
            if (Base64::Decode(json["Data"].get<EString>(), &data, &dataLen) &&
                Base64::Decode(json["UserData"].get<EString>(), &userData, &userDataLen))
            {
                if (resData->Data)
                {
                    delete resData->Data;
                    resData->Data = nullptr;
                }
                if (resData->UserData)
                {
                    delete resData->UserData;
                    resData->UserData = nullptr;
                }
                resData->Data = data;
                resData->DataSize = dataLen;
                resData->UserData = userData;
                resData->UserDataSize = userDataLen;
                return true;
            }
        }
        return !withData;
    }
    return false;
}

bool EDeserializer::ReadSceneFromFileBuffer(ESharedBuffer buffer, EDataBase* saveToScene) 
{
    EFileCollection fileCollection;
    fileCollection.SetFromCompleteBuffer(buffer);

    // Load resource
    saveToScene->GetResourceManager().Clear();

    EJson sceneJson = EJson::object();
    for (const auto& entry : fileCollection)
    {
        if (entry.first == "CORE/scene.json")
        {
            EString jsonString = EString(entry.second.Data<char>());
            
            sceneJson = EJson::parse(jsonString);
            continue;
        }


        const u8* pointer = entry.second.Data<u8>();

        EString type = EString((char*)pointer);
        pointer += type.length() + 1;

        EResourceData::t_ID id = *(EResourceData::t_ID*)pointer;
        pointer += sizeof(EResourceData::t_ID);
        
        u64 dataSize = *(u64*)pointer;
        pointer += sizeof(u64);

        u8* resourceData = new u8[dataSize];
        memcpy(resourceData, pointer, dataSize);
        pointer += dataSize;

        u64 userDataSize = *(u64*)pointer;
        u8* userData = nullptr;
        if (userDataSize > 0)
        {
            pointer += sizeof(u64);

            userData = new u8[userDataSize];
            memcpy(userData, pointer, userDataSize);
        }

        EFile file(entry.first);
        EResourceData* finalResourceData = new EResourceData(id, type, file.GetFileName(), resourceData, dataSize);
        finalResourceData->SetUserData(userData, userDataSize);
        if (!saveToScene->GetResourceManager().AddResource(finalResourceData))
        {
            E_ERROR("Could not add resource. Resource with same ids allready exist!");
            delete finalResourceData;
        }
    }

    return ReadSceneFromJson(sceneJson, saveToScene);
}