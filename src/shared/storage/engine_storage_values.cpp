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

ERef<EProperty> Engine::EProperty::Clone() const
{
    return this->OnClone();
}

void EProperty::Copy(const EProperty* from) 
{
    if (GetDescription() != from->GetDescription()) { E_ERROR("Could not copy properties. They have different types!"); return;}
    OnCopy(from);   
}

bool EProperty::IsValid() const
{
    return !fName.empty() && fDescription.Valid();
}

bool EProperty::operator()() const
{
    return IsValid();
}

void EProperty::SetChangeFunc(ChangeFunc func) 
{
    fChangeFunc = func;
}

void EProperty::ConnectChangeFunc(EProperty* other) 
{
    other->SetChangeFunc([this](EString ident){
        if (fChangeFunc)
        {
            fChangeFunc(GetPropertyName() + "." + ident);
        }
    });
}

void EProperty::Copy(const EProperty* copyFrom, EProperty* copyTo) 
{
    copyTo->Copy(copyFrom);
}



ERef<EProperty> EProperty::CreateFromDescription(const EString& name, EValueDescription description) 
{
    EValueType type = description.GetType();
    switch (type)
    {
    case EValueType::PRIMITIVE: return CreatePropertyPrimitive(name, description);
    case EValueType::ARRAY: return CreatePropertyArray(name, description);
    case EValueType::STRUCT: return CreatePropertyStruct(name, description);
    case EValueType::ENUM: return CreatePropertyEnum(name, description);
    case EValueType::ANY: return EMakeRef<EStructProperty>(name, description);
    case EValueType::UNKNOWN: return nullptr;
    }
    return nullptr;
}

ERef<EProperty> EProperty::CreatePropertyStruct(const EString& name, EValueDescription description)
{
    E_ASSERT_M(description.GetType() == EValueType::STRUCT, "Didnt provide struct description for creating property!");
    EVector<ERef<EProperty>> fields;
    for (auto& entry : description.GetStructFields())
    {
        ERef<EProperty> newField = CreateFromDescription(entry.first, entry.second);
        if (newField)
        {
            fields.push_back(newField);
        }
    }
    return EMakeRef<EStructProperty>(name, description, fields);
}

ERef<EProperty> EProperty::CreatePropertyPrimitive(const EString& name, EValueDescription description)
{
    E_ASSERT_M(description.GetType() == EValueType::PRIMITIVE, "Didnt provide primitive description for creating property!");
    const EString& primitiveId = description.GetId();
    if (primitiveId == E_TYPEID_STRING) { return EMakeRef<EValueProperty<EString>>(name, description); } 
    else if (primitiveId == E_TYPEID_INTEGER) { return EMakeRef<EValueProperty<i32>>(name, description); }
    else if (primitiveId == E_TYPEID_UNSIGNED_INTEGER) { return EMakeRef<EValueProperty<u32>>(name, description); }
    else if (primitiveId == E_TYPEID_UNSIGNED_BIG_INTEGER) { return EMakeRef<EValueProperty<u64>>(name, description); }
    else if (primitiveId == E_TYPEID_DOUBLE) { return EMakeRef<EValueProperty<double>>(name, description); }
    else if (primitiveId == E_TYPEID_FLOAT) { return EMakeRef<EValueProperty<float>>(name, description); }
    else if (primitiveId == E_TYPEID_BOOL) { return EMakeRef<EValueProperty<bool>>(name, description); }
    return nullptr;
}

ERef<EProperty> EProperty::CreatePropertyEnum(const EString& name, EValueDescription description)
{
    E_ASSERT_M(description.GetType() == EValueType::ENUM, "Didnt provide enum description for creating property!");
    ERef<EProperty> result = EMakeRef<EEnumProperty>(name, description);
    return result;
}

ERef<EProperty> EProperty::CreatePropertyArray(const EString& name, EValueDescription description)
{
    E_ASSERT_M(description.GetType() == EValueType::ARRAY, "Didnt provide enum description for creating property!");
    ERef<EProperty> result = EMakeRef<EArrayProperty>(name, description);
    return result;
}


EStructProperty::EStructProperty(const EString& name, EValueDescription description, const EVector<ERef<EProperty>>& properties)
    : EProperty(name, description), fProperties(properties)
{
    for (ERef<EProperty> property : fProperties)
    {
        ConnectChangeFunc(property.get());
    }
}

Engine::EStructProperty::EStructProperty(const EStructProperty& other) 
    : EProperty(other.fName, other.fDescription)
{
    fProperties.clear();
    for (ERef<EProperty> property : other.fProperties)
    {
        ERef<EProperty> clone = property->Clone();
        ConnectChangeFunc(clone.get());
        fProperties.push_back(clone);
    }
}

EStructProperty::~EStructProperty() 
{
    ResetFields();
}


bool EStructProperty::HasProperty(const EString& propertyName) const
{
    for (ERef<EProperty> prop : fProperties)
    {
        if (prop->GetPropertyName() == propertyName)
        {
            return true;
        }
    }
    return false;
}

ERef<EProperty> EStructProperty::GetProperty(const EString& propertyName) 
{
    for (ERef<EProperty> property : fProperties)
    {
        if (property->GetPropertyName() == propertyName)
        {
            return property;
        }
    }
    return nullptr;
}

const ERef<EProperty> EStructProperty::GetProperty(const EString& propertyName) const
{
    for (const ERef<EProperty> property : fProperties)
    {
        if (property->GetPropertyName() == propertyName)
        {
            return property;
        }
    }
    return nullptr;
}

ERef<EProperty> EStructProperty::GetPropertyByIdentifier(const EString& ident) const
{
    EVector<EString> identList = EStringUtil::SplitString(ident, ".");
    if (identList.size() == 0) { return nullptr; }

    ERef<EProperty> currentProp = GetProperty(identList[0]);
    for (size_t i = 1; i < identList.size(); i++)
    {
        if (!currentProp) { return nullptr; }
        const EString& currentIdent = identList[i];
        EValueDescription currentDsc = currentProp->GetDescription();
        switch (currentDsc.GetType())
        {
        case EValueType::PRIMITIVE:
        {
            break;
        }
        case EValueType::ARRAY:
        {
            if (std::regex_match(currentIdent, std::regex("[0-9]+", std::regex::ECMAScript)))
            {
                size_t arrayIndex = std::atoi(currentIdent.c_str());
                currentProp = std::dynamic_pointer_cast<EArrayProperty>(currentProp)->GetElement(arrayIndex);
            }
            else
            {
                currentProp = nullptr;
            }
            break;
        }
        case EValueType::STRUCT:
        {
            EStructProperty* structProp = static_cast<EStructProperty*>(currentProp.get());
            currentProp = structProp->GetProperty(currentIdent);
            break;
        }
        case EValueType::ANY:
        case EValueType::ENUM:
        {
            break;
        }
        case EValueType::UNKNOWN:
        {
            break;
        }
        }
    }
    return currentProp;
}

ERef<EProperty> Engine::EStructProperty::OnClone() const
{
    return EMakeRef<EStructProperty>(*this);
}

void EStructProperty::OnCopy(const EProperty* from) 
{
    //E_ASSERT_M(from->GetDescription().GetType() != EValueType::STRUCT && from->GetDescription().GetType() != EValueType::ANY, "Cant copy struct property from non-struct property!");
    if (from->GetDescription().GetType() != EValueType::STRUCT && from->GetDescription().GetType() != EValueType::ANY) 
    { 
        return; 
    }
    const EStructProperty* fromAsStruct = static_cast<const EStructProperty*>(from);
    for (const ERef<EProperty> prop : fromAsStruct->fProperties)
    {
        ERef<EProperty> copyToField = GetProperty(prop->GetPropertyName());
        if (copyToField)
        {
            copyToField->Copy(prop.get());
        }
        else
        {
            ERef<EProperty> newValue = prop->Clone();
            ConnectChangeFunc(newValue.get());
            fProperties.push_back(newValue);
        }
    }
}

void EStructProperty::ResetFields()
{
    fProperties.clear();
}

EEnumProperty::EEnumProperty(const EString& name, EValueDescription description, const EString& initValue)
    : EProperty(name, description), fValue(0)
{
    SetCurrentValueOption(initValue);
}

EEnumProperty::~EEnumProperty() 
{
    
}

u32 EEnumProperty::GetCurrentValue() const
{
    return fValue;
}

ERef<EProperty> EEnumProperty::OnClone() const
{
    return EMakeRef<EEnumProperty>(*this);
}

void EEnumProperty::OnCopy(const EProperty* from) 
{
    fValue = static_cast<const EEnumProperty*>(from)->fValue;
}

EArrayProperty::EArrayProperty(const EString& name, EValueDescription description)
    : EProperty(name, description)
{
    
}

Engine::EArrayProperty::EArrayProperty(const EArrayProperty& other) 
    : EProperty(other.fName, other.fDescription)
{
    for (ERef<EProperty> prop : other.fElements)
    {
        ERef<EProperty> clone = prop->Clone();
        ConnectChangeFunc(clone.get());
        fElements.push_back(clone);
    }
}

EArrayProperty::~EArrayProperty() 
{
    Clear();
}

ERef<EProperty> EArrayProperty::AddElement() 
{
    EString elementName = std::to_string(fElements.size());
    ERef<EProperty> result = EProperty::CreateFromDescription(elementName, fDescription.GetAsPrimitive());
    ConnectChangeFunc(result.get());
    fElements.push_back(result);
    if (fChangeFunc) {fChangeFunc(GetPropertyName());}
    return result;
}

ERef<EProperty> EArrayProperty::GetElement(size_t index) 
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
    fElements.erase(fElements.begin() + index);
    if (fChangeFunc) { fChangeFunc(GetPropertyName()); }
}

const EVector<ERef<EProperty>>& EArrayProperty::GetElements() const
{
    return fElements;
}

void EArrayProperty::Clear() 
{
    fElements.clear();
}

ERef<EProperty> EArrayProperty::OnClone() const
{
    return EMakeRef<EArrayProperty>(*this);
}

void EArrayProperty::OnCopy(const EProperty* from) 
{
    const EArrayProperty* fromAsArray = static_cast<const EArrayProperty*>(from);
    size_t smallestSize = E_MIN(fElements.size(), fromAsArray->fElements.size());
    for (size_t i = 0; i < fromAsArray->fElements.size(); i++)
    {
        if (fElements.size() > i)
        {
            fElements[i]->Copy(fromAsArray->fElements[i].get());
        }
        else
        {
            ERef<EProperty> newValue = fromAsArray->fElements[i]->Clone();
            ConnectChangeFunc(newValue.get());
            fElements.push_back(newValue);
        }
    }
}

