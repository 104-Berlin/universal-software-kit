#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

EDataType EDataHandle::data_type = EDataType::UNKNOWN;
EDataType EIntegerDataHandle::data_type = EDataType::INTEGER;
EDataType EFloatDataHandle::data_type = EDataType::FLOAT;
EDataType EBooleanDataHandle::data_type = EDataType::BOOLEAN;
EDataType EStringDataHandle::data_type = EDataType::STRING;
EDataType EStructureDataHandle::data_type = EDataType::STRUCTURE;



EDataHandle* Handle::CreateDataFromDescpription(const EStructureDescription& description) 
{
    switch (description.GetDataType())
    {
    case EDataType::UNKNOWN:
        E_WARN("Cant create unknown data type!");
        break;
    case EDataType::INTEGER:
        return new EIntegerDataHandle(description.GetDataName());
    case EDataType::FLOAT:
        return new EFloatDataHandle(description.GetDataName());
    case EDataType::BOOLEAN:
        return new EBooleanDataHandle(description.GetDataName());
        break;
    case EDataType::STRING:
        return new EStringDataHandle(description.GetDataName());
    case EDataType::STRUCTURE:
        return new EStructureDataHandle(description.GetDataName(), description);
    }
    return nullptr;
}






EDataHandle::EDataHandle(const EString& name, EDataType type)
    : fDataDescription({name, type})
{
    
}

const EString& EDataHandle::GetName() const
{
    return fDataDescription.GetDataName();   
}

EDataType EDataHandle::GetDataType() const
{
    return fDataDescription.GetDataType();
}

EIntegerDataHandle::EIntegerDataHandle(const EString& name, i32 defaultValue) 
    : EDataHandle(name, EDataType::INTEGER), fValue(defaultValue)
{
    
}

i32 EIntegerDataHandle::GetValue() const
{
    return fValue;
}

void EIntegerDataHandle::SetValue(i32 value) 
{
    fValue = value;
}

EIntegerDataHandle::operator i32() const
{
    return fValue;
}

void EIntegerDataHandle::operator=(i32 value) 
{
    fValue = value;
}

EFloatDataHandle::EFloatDataHandle(const EString& name, float defaultValue) 
    : EDataHandle(name, EDataType::FLOAT), fValue(defaultValue)
{
    
}

float EFloatDataHandle::GetValue() const
{
    return fValue;
}

void EFloatDataHandle::SetValue(float value) 
{
    fValue = value;
}

EFloatDataHandle::operator float() const
{
    return fValue;
}

void EFloatDataHandle::operator=(float value) 
{
    fValue = value;
}

EBooleanDataHandle::EBooleanDataHandle(const EString& name, bool defaultValue) 
    : EDataHandle(name, EDataType::BOOLEAN), fValue(defaultValue)
{
    
}

bool EBooleanDataHandle::GetValue() const
{
    return fValue;
}

void EBooleanDataHandle::SetValue(bool value) 
{
    fValue = value;
}

EBooleanDataHandle::operator bool() const
{
    return fValue;
}

void EBooleanDataHandle::operator=(bool value) 
{
    fValue = value;
}

EStringDataHandle::EStringDataHandle(const EString& name, const EString& defaultValue) 
    : EDataHandle(name, EDataType::STRING), fValue(defaultValue)
{
    
}

const EString& EStringDataHandle::GetValue() const
{
    return fValue;    
}

void EStringDataHandle::SetValue(const EString& value) 
{
    fValue = value;
}

EStringDataHandle::operator const EString&() const
{
    return fValue;
}

void EStringDataHandle::operator=(const EString& value) 
{
    fValue = value;
}

EStructureDescription::EStructureDescription(const EString& name, EDataType type, const EVector<EStructureDescription>&  childs) 
    : fDataName(name), fDataType(type)
{
    if (type != EDataType::STRUCTURE)
    {
        E_ASSERT(childs.size() == 0, "Can't describe non structure value with childs. The childs will be ignored! " + name); // If not structure type we wont expept any childs
    }
    else
    {
        fChilds = childs;
    }
}

const EString& EStructureDescription::GetDataName() const
{
    return fDataName;
}

EDataType EStructureDescription::GetDataType() const
{
    return fDataType;
}

const EVector<EStructureDescription>& EStructureDescription::GetChilds() const
{
    return fChilds;
}

EStructureDataHandle::EStructureDataHandle(const EString& name, const EStructureDescription& description) 
    : EDataHandle(name, EDataType::STRUCTURE)
{
    E_ASSERT(description.GetDataType() == EDataType::STRUCTURE, "Can't init structure data with non structure description");
    if (description.GetDataType() == EDataType::STRUCTURE)
    {
        for (const auto& entry : description.GetChilds())
        {
            EDataHandle* newFieldPtr = Handle::CreateDataFromDescpription(entry);
            if (newFieldPtr)
            {
                fFields.insert({newFieldPtr->GetName(), ERef<EDataHandle>(newFieldPtr)});
            }
        }
    }
}

EStructureDataHandle::~EStructureDataHandle() 
{
    fFields.clear();
}

ERef<EDataHandle> EStructureDataHandle::GetFieldAt(const EString& name) 
{
    if (!HasFieldAt(name)) { return nullptr; }
    return fFields.at(name);
}

bool EStructureDataHandle::HasFieldAt(const EString& name) 
{
    return fFields.find(name) != fFields.end();
}


const EStructureDataHandle::FieldMap& EStructureDataHandle::GetFields() 
{
    return fFields;
}

EStructureDataHandle::FieldMap::iterator EStructureDataHandle::begin() 
{
    return fFields.begin();
}

EStructureDataHandle::FieldMap::iterator EStructureDataHandle::end() 
{
    return fFields.end();
}

EStructureDataHandle::FieldMap::const_iterator EStructureDataHandle::begin() const
{
    return fFields.begin();
}

EStructureDataHandle::FieldMap::const_iterator EStructureDataHandle::end() const
{
    return fFields.end();
}

