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



EProperty* EProperty::CreateFromDescription(const EString& name, EValueDescription description) 
{
    EValueType type = description.GetType();
    switch (type)
    {
    case EValueType::PRIMITIVE: return CreatePropertyPrimitive(name, description);
    case EValueType::ARRAY: return CreatePropertyArray(name, description);
    case EValueType::STRUCT: return CreatePropertyStruct(name, description);
    case EValueType::ENUM: return CreatePropertyEnum(name, description);
    case EValueType::UNKNOWN: return nullptr;
    }
    return nullptr;
}

EProperty* EProperty::CreatePropertyStruct(const EString& name, EValueDescription description)
{
    E_ASSERT_M(description.GetType() == EValueType::STRUCT, "Didnt provide struct description for creating property!");
    EVector<EProperty*> fields;
    for (auto& entry : description.GetStructFields())
    {
        EProperty* newField = CreateFromDescription(entry.first, entry.second);
        if (newField)
        {
            fields.push_back(newField);
        }
    }
    return new EStructProperty(name, description, fields);
}

EProperty* EProperty::CreatePropertyPrimitive(const EString& name, EValueDescription description)
{
    E_ASSERT_M(description.GetType() == EValueType::PRIMITIVE, "Didnt provide primitive description for creating property!");
    const EString& primitiveId = description.GetId();
    if (primitiveId == E_TYPEID_STRING) { return new EValueProperty<EString>(name, description); } 
    else if (primitiveId == E_TYPEID_INTEGER) { return new EValueProperty<i32>(name, description); }
    else if (primitiveId == E_TYPEID_UNSIGNED_INTEGER) { return new EValueProperty<u32>(name, description); }
    else if (primitiveId == E_TYPEID_UNSIGNED_BIG_INTEGER) { return new EValueProperty<u64>(name, description); }
    else if (primitiveId == E_TYPEID_DOUBLE) { return new EValueProperty<double>(name, description); }
    else if (primitiveId == E_TYPEID_FLOAT) { return new EValueProperty<float>(name, description); }
    else if (primitiveId == E_TYPEID_BOOL) { return new EValueProperty<bool>(name, description); }
    return nullptr;
}

EProperty* EProperty::CreatePropertyEnum(const EString& name, EValueDescription description)
{
    E_ASSERT_M(description.GetType() == EValueType::ENUM, "Didnt provide enum description for creating property!");
    EEnumProperty* result = new EEnumProperty(name, description);
    return result;
}

EProperty* EProperty::CreatePropertyArray(const EString& name, EValueDescription description)
{
    E_ASSERT_M(description.GetType() == EValueType::ARRAY, "Didnt provide enum description for creating property!");
    EArrayProperty* result = new EArrayProperty(name, description);
    return result;
}


EStructProperty::EStructProperty(const EString& name, EValueDescription description, const EVector<EProperty*>& properties)
    : EProperty(name, description), fProperties(properties)
{
    for (EProperty* property : fProperties)
    {
        ConnectChangeFunc(property);
    }
}

Engine::EStructProperty::EStructProperty(const EStructProperty& other) 
    : EProperty(other.fName, other.fDescription)
{
    fProperties.clear();
    for (EProperty* property : other.fProperties)
    {
        EProperty* clone = property->Clone();
        ConnectChangeFunc(clone);
        fProperties.push_back(clone);
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

EProperty* EStructProperty::GetPropertyByIdentifier(const EString& ident) const
{
    EVector<EString> identList = EStringUtil::SplitString(ident, ".");
    EProperty* currentProp = (EProperty*) this;
    for (size_t i = 0; i < identList.size(); i++)
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
                currentProp = static_cast<EArrayProperty*>(currentProp)->GetElement(arrayIndex);
            }
            else
            {
                currentProp = nullptr;
            }
            break;
        }
        case EValueType::STRUCT:
        {
            EStructProperty* structProp = static_cast<EStructProperty*>(currentProp);
            currentProp = structProp->GetProperty(currentIdent);
            break;
        }
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

EProperty* Engine::EStructProperty::OnClone() 
{
    return new EStructProperty(*this);
}

void EStructProperty::OnCopy(const EProperty* from) 
{
    for (EProperty* prop : fProperties)
    {
        const EProperty* copyFromField = static_cast<const EStructProperty*>(from)->GetProperty(prop->GetPropertyName());
        if (copyFromField)
        {
            prop->Copy(copyFromField);
        }
    }
}

EEnumProperty::EEnumProperty(const EString& name, EValueDescription description, const EString& initValue)
    : EProperty(name, description), fValue(0)
{
    SetCurrentValue(initValue);
}

EEnumProperty::~EEnumProperty() 
{
    
}

void EEnumProperty::SetCurrentValue(const EString& value) 
{
    if (value.empty())
    {
        fValue = 0;
        return;
    }
    EValueDescription dsc = GetDescription();
    E_ASSERT(dsc.GetType() == EValueType::ENUM);
    const EVector<EString>& options = dsc.GetEnumOptions();
    EVector<EString>::const_iterator foundOption = std::find(options.begin(), options.end(), value);
    if (foundOption == options.end())
    {
        fValue = 0;
    }
    else
    {
        fValue = std::distance(options.begin(), foundOption);
    }

    if (fChangeFunc) { fChangeFunc(GetPropertyName()); }
}


void EEnumProperty::SetCurrentValue(u32 value)
{
    EValueDescription dsc = GetDescription();
    E_ASSERT(dsc.GetType() == EValueType::ENUM);
    const EVector<EString>& options = dsc.GetEnumOptions();
    if (value >= options.size())
    {
        fValue = 0;
    }
    else
    {
        fValue = value;
    }

    if (fChangeFunc) { fChangeFunc(GetPropertyName()); }
}

u32 EEnumProperty::GetCurrentValue() const
{
    return fValue;
}

EProperty* EEnumProperty::OnClone() 
{
    return new EEnumProperty(*this);
}

void EEnumProperty::OnCopy(const EProperty* from) 
{
    fValue = static_cast<const EEnumProperty*>(from)->fValue;
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
        EProperty* clone = prop->Clone();
        ConnectChangeFunc(clone);
        fElements.push_back(clone);
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
    ConnectChangeFunc(result);
    fElements.push_back(result);
    if (fChangeFunc) {fChangeFunc(GetPropertyName());}
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
    if (fChangeFunc) { fChangeFunc(GetPropertyName()); }
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

void EArrayProperty::OnCopy(const EProperty* from) 
{
    const EArrayProperty* fromAsArray = static_cast<const EArrayProperty*>(from);
    size_t smallestSize = E_MIN(fElements.size(), fromAsArray->fElements.size());
    for (size_t i = 0; i < fromAsArray->fElements.size(); i++)
    {
        if (fElements.size() > i)
        {
            fElements[i]->Copy(fromAsArray->fElements[i]);
        }
        else
        {
            fElements.push_back(fromAsArray->fElements[i]->Clone());
        }
    }
}

