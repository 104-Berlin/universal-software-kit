#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

EValueDescription::EValueDescription(EValueType type, EValueDescription::t_ID id) 
    : fType(type), fID(id), fArrayType(nullptr), fDefaultValue(nullptr)
{
    if (type == EValueType::ARRAY)
    {
        fArrayType = new EValueDescription();
    }
}

EValueDescription::EValueDescription(const EValueDescription& other)
    : fType(other.fType), fID(other.fID), fArrayType(nullptr), fEnumOptions(other.fEnumOptions)
{
    fStructFields.clear();
    for (auto& entry : other.fStructFields)
    {
        fStructFields.push_back({entry.first, new EValueDescription(*entry.second)});
    }

    fDefaultValue = other.fDefaultValue;

    if (other.fArrayType)
    {
        fArrayType = new EValueDescription(*other.fArrayType);
    }

    for (EValueDescription* depends : other.fDependsOn)
    {
        fDependsOn.push_back(new EValueDescription(*depends));
    }
}


EValueDescription& EValueDescription::operator=(const EValueDescription& other)
{
    fType = other.fType;
    fID = other.fID;
    fEnumOptions = other.fEnumOptions;
    fDefaultValue = other.fDefaultValue;

    fStructFields.clear();
    for (auto& entry : other.fStructFields)
    {
        fStructFields.push_back({entry.first, new EValueDescription(*entry.second)});
    }

    if (other.fArrayType)
    {
        fArrayType = new EValueDescription(*other.fArrayType);
    }
    for (EValueDescription* depends : other.fDependsOn)
    {
        fDependsOn.push_back(new EValueDescription(*depends));
    }
    return *this;
}


EValueDescription::~EValueDescription() 
{
    for (auto& entry : fStructFields)
    {
        delete entry.second;
    }
    if (fArrayType)
    {
        delete fArrayType;
    }
    for (auto& entry : fDependsOn)
    {
        delete entry;
    }
}

EValueType EValueDescription::GetType() const
{
    return fType;   
}

const EValueDescription::t_ID& EValueDescription::GetId() const
{
    return fID;
}

bool EValueDescription::Valid() const
{
    return fType != EValueType::UNKNOWN && !fID.empty();
}


EValueDescription EValueDescription::GetAsArray() const
{
    EValueDescription result(EValueType::ARRAY, GetId());
    result.SetArrayType(*this);
    return result;
}

EValueDescription EValueDescription::GetAsPrimitive() const
{
    return *fArrayType;
}

void EValueDescription::SetDefaultValue(EProperty* value)
{
    if (!value)
    {
        E_WARN("Default value is nullptr");
        return;
    }
    if (!fDefaultValue)
    {
        fDefaultValue = EProperty::CreateFromDescription(GetId(), *this);
    }
    fDefaultValue->Copy(value);
}

ERef<EProperty> EValueDescription::GetDefaultValue() const
{
    return fDefaultValue;
}

void EValueDescription::AddDependsOn(const EValueDescription& value)
{
    fDependsOn.push_back(new EValueDescription(value));
}

EVector<EValueDescription> EValueDescription::GetDependsOn() const
{
    EVector<EValueDescription> result;
    for (auto& entry : fDependsOn)
    {
        result.push_back(EValueDescription(*entry));
    }
    return result;  
}


EValueDescription EValueDescription::CreateStruct(const t_ID& id,  EVector<StructField> childs, EProperty* defaultValue) 
{
    EValueDescription result(EValueType::STRUCT, id);
    if (id == "ETransform")
    {
        E_INFO("ETransform");
    }
    for (const StructField& entry : childs)
    {
        result.AddStructField(entry.first, entry.second);
    }
    if (defaultValue)
    {
        result.SetDefaultValue(defaultValue);
    }
    return result;
}

EValueDescription EValueDescription::CreateEnum(const t_ID& id, EVector<EString> options)
{
    EValueDescription result(EValueType::ENUM, id);
    for (const EString& entry : options)
    {
        if (!entry.empty())
        {
            result.AddEnumOption(entry);
        }
    }
    return result;
}


bool EValueDescription::operator==(const EValueDescription& other) const
{
    return fID == other.fID && fType == other.fType && (fType != EValueType::ARRAY || *fArrayType == *other.fArrayType);
}

bool EValueDescription::operator!=(const EValueDescription& other) const
{
    return !((*this) == other);
}

EValueDescription& EValueDescription::AddStructField(const EString& name, EValueDescription description) 
{
    if (fType != EValueType::STRUCT) 
    {
        E_WARN("WARN: Can't add a field to a non struct");
        return *this;
    }
    fStructFields.push_back({name, new EValueDescription(description)});
    return *this;
}

EVector<EValueDescription::StructField> EValueDescription::GetStructFields() const
{
    EVector<StructField> result;
    for (auto& entry : fStructFields)
    {
        result.push_back({entry.first, *entry.second});
    }
    return result;    
}

EValueDescription& EValueDescription::AddEnumOption(const EString& option) 
{
    if (fType != EValueType::ENUM)
    {
        E_WARN("WARN: Can't add enum-option to non enum!");
        return *this;
    }
    fEnumOptions.push_back(option);
    return *this;
}

const EVector<EString>& EValueDescription::GetEnumOptions() const
{
    return fEnumOptions;
}

void EValueDescription::SetArrayType(const EValueDescription& type) 
{
    if (GetType() == EValueType::ARRAY)
    {
        if (!fArrayType) { fArrayType = new EValueDescription(); }

        *fArrayType = type;
    }
}

