#include "engine.h"
#include "prefix_shared.h"


Engine::EValueDescription* Engine::StringDescription() 
{
        static EValueDescription* result = new EValueDescription(EValueType::PRIMITIVE, E_TYPEID_STRING);
        return result;

}

Engine::EValueDescription* Engine::IntegerDescription() 
{
    static EValueDescription* result = new EValueDescription(EValueType::PRIMITIVE, E_TYPEID_INTEGER);
    return result;
}

Engine::EValueDescription* Engine::DoubleDescription() 
{
    static EValueDescription* result = new EValueDescription(EValueType::PRIMITIVE, E_TYPEID_DOUBLE);
    return result;
}

Engine::EValueDescription* Engine::BoolDescription() 
{
    static EValueDescription* result = new EValueDescription(EValueType::PRIMITIVE, E_TYPEID_BOOL);
    return result;
}



using namespace Engine;

EValueDescription::EValueDescription(EValueType type, EString id) 
    : fType(type), fID(id)
{
    ETypeRegister::get().RegisterDescription(this);
}

EValueDescription::~EValueDescription() 
{
    
}

void ETypeRegister::RegisterDescription(EValueDescription* description) 
{
    E_ASSERT(description, "Cant register undefined description");
    fRegisteredDescriptions[description->GetId()] = description;
}

EValueDescription* ETypeRegister::FindById(const EString& id) 
{
    return fRegisteredDescriptions[id];
}

EVector<EValueDescription*> ETypeRegister::GetAllDescriptions() 
{
    EVector<EValueDescription*> result;
    for (const auto& entry : fRegisteredDescriptions)
    {
        result.push_back(entry.second);
    }
    return result;
}

EValueType EValueDescription::GetType() const
{
    return fType;   
}

const EString& EValueDescription::GetId() const
{
    return fID;
}

EStructDescription::EStructDescription(const EString& name) 
    : EValueDescription(EValueType::STRUCT, name)
{
    
}

EStructDescription::~EStructDescription() 
{
    
}

EStructDescription& EStructDescription::AddField(const EString& name, EValueDescription* description) 
{
    fFields[name] = description;
    return *this;
}

const EUnorderedMap<EString, EValueDescription*>& EStructDescription::GetFields() const
{
    return fFields;    
}

EEnumDescription::EEnumDescription(const EString& name) 
    : EValueDescription(EValueType::ENUM, name)
{
    
}

EEnumDescription::~EEnumDescription() 
{
    
}

EEnumDescription& EEnumDescription::AddOption(const EString& option) 
{
    fOptions.push_back(option);
    return *this;
}

const EVector<EString>& EEnumDescription::GetOptions() const
{
    return fOptions;
}

EArrayDescription::EArrayDescription(EValueDescription* arrayType) 
    : EValueDescription(EValueType::ARRAY, arrayType->GetId() + "List"), fType(arrayType)
{
    E_ASSERT(arrayType, "ERROR: Array need type descpription!");
}

EArrayDescription::~EArrayDescription() 
{
    
}

EValueDescription* EArrayDescription::GetElementType() const
{
    return fType;
}

ETypeRegister::ETypeRegister() 
{
    
}

ETypeRegister::~ETypeRegister() 
{
    for (auto& entry : fRegisteredDescriptions)
    {
        delete entry.second;
    }
}

// static runner to init primitive types
template <typename T>
struct static_runner
{
    static_runner(T&& t)
    {
        t();
    }
};

static static_runner runnder([](){
    StringDescription();
    IntegerDescription();
    DoubleDescription();
    BoolDescription();
});