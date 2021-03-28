#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

EDataType EDataHandle::data_type = EDataType::UNKNOWN;
EDataType EIntegerDataHandle::data_type = EDataType::INTEGER;
EDataType EFloatDataHandle::data_type = EDataType::FLOAT;
EDataType EBooleanDataHandle::data_type = EDataType::BOOLEAN;
EDataType EStringDataHandle::data_type = EDataType::STRING;
EDataType EStructureDataHandle::data_type = EDataType::STRUCTURE;


EDataHandle::EDataHandle(const EString& name, EDataType type)
    : fDataDescription({name, type})
{
    
}

const EString& EDataHandle::GetName() const
{
    return fDataDescription.DataName;   
}

EDataType EDataHandle::GetDataType() const
{
    return fDataDescription.DataType;
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


EStructureDataHandle::EStructureDataHandle(const EString& name, const EStructureDescription& description) 
    : EDataHandle(name, EDataType::STRUCTURE)
{
    
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

void EStructureDataHandle::AddFieldsFromDescpription(const EStructureDescription& description) 
{
    if (description.TypeData.DataType == EDataType::STRUCTURE)
    {
        for (auto child : description.Childs)
        {
            
        }
    }
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

