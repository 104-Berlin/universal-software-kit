#include "prefix_interface.h"

using namespace Engine;

void PrintProperty_Prim(EProperty* primitive)
{
    EValueDescription dsc = primitive->GetDescription();
    const EString& primitiveId = dsc.GetId();
    if (primitiveId == E_TYPEID_STRING) { std::cout << "\"" << static_cast<EValueProperty<EString>*>(primitive)->GetValue() << "\""; } 
    else if (primitiveId == E_TYPEID_INTEGER) { std::cout << (static_cast<EValueProperty<i32>*>(primitive))->GetValue(); }
    else if (primitiveId == E_TYPEID_UNSIGNED_INTEGER) { std::cout << (static_cast<EValueProperty<u32>*>(primitive))->GetValue(); }
    else if (primitiveId == E_TYPEID_UNSIGNED_BIG_INTEGER) { std::cout << (static_cast<EValueProperty<u64>*>(primitive))->GetValue(); }
    else if (primitiveId == E_TYPEID_DOUBLE) { std::cout << (static_cast<EValueProperty<double>*>(primitive))->GetValue(); }
    else if (primitiveId == E_TYPEID_BOOL) { std::cout << (static_cast<EValueProperty<bool>*>(primitive))->GetValue(); }
}

void PrintProperty_Struct(EStructProperty* prop)
{
    EValueDescription dsc = prop->GetDescription();
    std::cout << std::endl;
    for (auto& entry : dsc.GetStructFields())
    {
        EProperty* childProp = prop->GetProperty(entry.first);
        if (childProp)
        {
            std::cout << "\t";
            inter::PrintProperty(childProp);
        }
    }
}

void PrintProperty_Array(EArrayProperty* prop)
{
    std::cout << "[";
    for (auto& entry : prop->GetElements())
    {
        inter::PrintProperty(entry);
        std::cout << ",";
    }
    std::cout << "]";
}

void inter::PrintProperty(EProperty* prop) 
{
    EValueDescription dsc = prop->GetDescription();
    std::cout << "\"" << prop->GetPropertyName() << "\": ";
    
    switch (dsc.GetType())
    {
    case EValueType::PRIMITIVE: PrintProperty_Prim(prop); break;
    case EValueType::ARRAY: PrintProperty_Array(static_cast<EArrayProperty*>(prop)); break;
    case EValueType::STRUCT: PrintProperty_Struct(static_cast<EStructProperty*>(prop)); break;
    case EValueType::ENUM: break;
    case EValueType::UNKNOWN: break;
    }
    
    std::cout << std::endl;
}
