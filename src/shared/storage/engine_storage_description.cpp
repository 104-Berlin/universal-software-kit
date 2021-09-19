#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

EValueDescription::EValueDescription(EValueType type, EValueDescription::t_ID id) 
    : fType(type), fID(id), fArrayType(nullptr)
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
        fStructFields[entry.first] = new EValueDescription(*entry.second);
    }

    if (other.fArrayType)
    {
        fArrayType = new EValueDescription(*other.fArrayType);
    }
}


EValueDescription& EValueDescription::operator=(const EValueDescription& other)
{
    fType = other.fType;
    fID = other.fID;
    fEnumOptions = other.fEnumOptions;

    fStructFields.clear();
    for (auto& entry : other.fStructFields)
    {
        fStructFields[entry.first] = new EValueDescription(*entry.second);
    }

    if (other.fArrayType)
    {
        fArrayType = new EValueDescription(*other.fArrayType);
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

EValueDescription EValueDescription::CreateStruct(const t_ID& id, std::initializer_list<std::pair<EString, EValueDescription>> childs) 
{
    EValueDescription result(EValueType::STRUCT, id);
    for (const std::pair<EString, EValueDescription>& entry : childs)
    {
        result.AddStructField(entry.first, entry.second);
    }
    return result;
}

bool EValueDescription::operator==(const EValueDescription& other) 
{
    return fID == other.fID && fType == other.fType;
}

bool EValueDescription::operator!=(const EValueDescription& other) 
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
    fStructFields[name] = new EValueDescription(description);
    return *this;
}

const EUnorderedMap<EString, EValueDescription*>& EValueDescription::GetStructFields() const
{
    return fStructFields;    
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

