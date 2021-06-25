#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

EValueDescription::EValueDescription(EValueType type, EValueDescription::t_ID id) 
    : fType(type), fID(id), fIsArray(false)
{

}

EValueDescription::EValueDescription(const EValueDescription& other)
    : fType(other.fType), fID(other.fID), fIsArray(other.fIsArray), fEnumOptions(other.fEnumOptions)
{
    fStructFields.clear();
    for (auto& entry : other.fStructFields)
    {
        fStructFields[entry.first] = new EValueDescription(*entry.second);
    }
}


EValueDescription& EValueDescription::operator=(const EValueDescription& other)
{
    fType = other.fType;
    fID = other.fID;
    fIsArray = other.fIsArray;
    fEnumOptions = other.fEnumOptions;

    fStructFields.clear();
    for (auto& entry : other.fStructFields)
    {
        fStructFields[entry.first] = new EValueDescription(*entry.second);
    }
    return *this;
}


EValueDescription::~EValueDescription() 
{
    for (auto& entry : fStructFields)
    {
        delete entry.second;
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

bool EValueDescription::IsArray() const
{
    return fIsArray;
}

EValueDescription EValueDescription::GetAsArray() const
{
    EValueDescription result(*this);
    result.fIsArray = true;
    return result;
}

EValueDescription EValueDescription::GetAsPrimitive() const
{
    EValueDescription result(*this);
    result.fIsArray = false;
    return result;
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