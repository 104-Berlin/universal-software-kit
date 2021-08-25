#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;



EProperty::EProperty(const EString& name, EValueDescription description) 
    : fName(name), fDescription(description)
{
    
}

const EString& EProperty::GetPropertyName() const
{
    return fName;
}

EValueDescription EProperty::GetDescription() const
{
    return fDescription;
}

EProperty* Engine::EProperty::Clone() 
{
    return this->OnClone();
}



EProperty* EProperty::CreateFromDescription(const EString& name, EValueDescription description) 
{
    EValueType type = description.GetType();
    if (description.IsArray())
    {
        return CreatePropertyArray(name, description);
    }
    else
    {
        switch (type)
        {
        case EValueType::PRIMITIVE: return CreatePropertyPrimitive(name, description);
        case EValueType::STRUCT: return CreatePropertyStruct(name, description);
        case EValueType::ENUM: return CreatePropertyEnum(name, description);
        case EValueType::UNKNOWN: return nullptr;
        }
    }
    return nullptr;
}

EProperty* EProperty::CreatePropertyStruct(const EString& name, EValueDescription description)
{
    E_ASSERT(description.GetType() == EValueType::STRUCT, "Didnt provide struct description for creating property!");
    EVector<EProperty*> fields;
    for (auto& entry : description.GetStructFields())
    {
        EProperty* newField = CreateFromDescription(entry.first, *entry.second);
        if (newField)
        {
            fields.push_back(newField);
        }
    }
    return new EStructProperty(name, description, fields);
}

EProperty* EProperty::CreatePropertyPrimitive(const EString& name, EValueDescription description)
{
    E_ASSERT(description.GetType() == EValueType::PRIMITIVE, "Didnt provide primitive description for creating property!");
    const EString& primitiveId = description.GetId();
    if (primitiveId == E_TYPEID_STRING) { return new EValueProperty<EString>(name, description); } 
    else if (primitiveId == E_TYPEID_INTEGER) { return new EValueProperty<i32>(name, description); }
    else if (primitiveId == E_TYPEID_DOUBLE) { return new EValueProperty<double>(name, description); }
    else if (primitiveId == E_TYPEID_BOOL) { return new EValueProperty<bool>(name, description); }
    return nullptr;
}

EProperty* EProperty::CreatePropertyEnum(const EString& name, EValueDescription description)
{
    E_ASSERT(description.GetType() == EValueType::ENUM, "Didnt provide enum description for creating property!");
    EEnumProperty* result = new EEnumProperty(name, description);
    return result;
}

EProperty* EProperty::CreatePropertyArray(const EString& name, EValueDescription description)
{
    E_ASSERT(description.IsArray(), "Didnt provide enum description for creating property!");
    EArrayProperty* result = new EArrayProperty(name, description);
    return result;
}


EStructProperty::EStructProperty(const EString& name, EValueDescription description, const EVector<EProperty*>& properties)
    : EProperty(name, description), fProperties(properties)
{
    
}

Engine::EStructProperty::EStructProperty(const EStructProperty& other) 
    : EProperty(other.fName, other.fDescription)
{
    fProperties.clear();
    for (EProperty* property : other.fProperties)
    {
        fProperties.push_back(property->Clone());
    }
}

EStructProperty::~EStructProperty() 
{
    for (EProperty* property : fProperties)
    {
        delete property;
    }
    fProperties.clear();
}

bool EStructProperty::HasProperty(const EString& propertyName) const
{
    for (EProperty* prop : fProperties)
    {
        if (prop->GetPropertyName() == propertyName)
        {
            return true;
        }
    }
    return false;
}

EProperty* EStructProperty::GetProperty(const EString& propertyName) 
{
    for (EProperty* property : fProperties)
    {
        if (property->GetPropertyName() == propertyName)
        {
            return property;
        }
    }
    return nullptr;
}

const EProperty* EStructProperty::GetProperty(const EString& propertyName) const
{
    for (const EProperty* property : fProperties)
    {
        if (property->GetPropertyName() == propertyName)
        {
            return property;
        }
    }
    return nullptr;
}

EProperty* Engine::EStructProperty::OnClone() 
{
    return new EStructProperty(*this);
}

EEnumProperty::EEnumProperty(const EString& name, EValueDescription description, const EString& initValue)
    : EProperty(name, description), fValue(initValue)
{
    const EVector<EString>& options = description.GetEnumOptions();
    if (options.size() > 0)
    {
        if (fValue.empty())
        {
            fValue = options[0];
        }
        else
        {
            EVector<EString>::const_iterator foundOption = std::find(options.begin(), options.end(), fValue);
            if (foundOption == options.end())
            {
                fValue = options[0];
            }
        }
    }
    else
    {
        fValue = "";
    }
}

EEnumProperty::~EEnumProperty() 
{
    
}

void EEnumProperty::SetCurrentValue(const EString& value) 
{
    fValue = value;
}

const EString& EEnumProperty::GetCurrentValue() const
{
    return fValue;
}

EProperty* EEnumProperty::OnClone() 
{
    return new EEnumProperty(*this);
}

EArrayProperty::EArrayProperty(const EString& name, EValueDescription description)
    : EProperty(name, description)
{
    
}

Engine::EArrayProperty::EArrayProperty(EArrayProperty& other) 
    : EProperty(other.fName, other.fDescription)
{
    for (EProperty* prop : other.fElements)
    {
        fElements.push_back(prop->Clone());
    }
}

EArrayProperty::~EArrayProperty() 
{
    Clear();
}

EProperty* EArrayProperty::AddElement() 
{
    EString elementName = std::to_string(fElements.size());
    EProperty* result = EProperty::CreateFromDescription(elementName, fDescription.GetAsPrimitive());
    fElements.push_back(result);
    return result;
}

EProperty* EArrayProperty::GetElement(size_t index) 
{
    if (index >= fElements.size()) 
    {
        E_ERROR("ERROR: Index out of bounds exception! Cant access element at index " + std::to_string(index) + " for array \"" + GetPropertyName() + "\"");
        return nullptr;
    }
    return fElements[index];
}

void EArrayProperty::RemoveElement(size_t index) 
{
    if (index >= fElements.size()) 
    {
        E_ERROR("ERROR: Index out of bounds exception! Cant delete element at index " + std::to_string(index) + " for array \"" + GetPropertyName() + "\"");
        return;
    }
    delete fElements[index];
    fElements.erase(fElements.begin() + index);
}

const EVector<EProperty*>& EArrayProperty::GetElements() const
{
    return fElements;
}

void EArrayProperty::Clear() 
{
    for (EProperty* property : fElements)
    {
        delete property;
    }
    fElements.clear();
}

EProperty* EArrayProperty::OnClone() 
{
    return new EArrayProperty(*this);
}
