#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

EDataHandle::EDataHandle(const EString& name, EDataType type)
    : fDataDescription({name, type})
{
    
}

EIntegerDataHandle::EIntegerDataHandle(const EString& name, i32 defaultValue) 
    : EDataHandle(name, EDataType::INTEGER), fValue(defaultValue)
{
    
}

EIntegerDataHandle::~EIntegerDataHandle() 
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

EStructureDataHandle::EStructureDataHandle(const EString& name) 
    : EDataHandle(name, EDataType::STRUCTURE)
{
    
}

EStructureDataHandle::~EStructureDataHandle() 
{
    
}

bool EStructureDataHandle::GetFieldAt(const EString& name) 
{
    return false;
}

void EStructureDataHandle::AddField(EDataDescriptor descriptor) 
{
    switch (descriptor.DataType)
    {
    case Engine::EDataType::UNKNOWN:
        E_WARN("Cant add unknown data type to StructureData called " + descriptor.DataName);
        break;
    case Engine::EDataType::INTEGER:
        fFields.insert({descriptor.DataName, EMakeRef<EIntegerDataHandle>(descriptor.DataName)});
        break;
    case Engine::EDataType::FLOAT:
        break;
    case Engine::EDataType::BOOLEAN:
        break;
    case Engine::EDataType::STRING:
        break;
    case Engine::EDataType::VECTOR2:
        break;
    case Engine::EDataType::VECTOR3:
        break;
    case Engine::EDataType::VECTOR4:
        break;
    case Engine::EDataType::ARRAY:
        break;
    case Engine::EDataType::STRUCTURE:
        break;
    case Engine::EDataType::STRUCTURE_REF:
        break;
    }
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

