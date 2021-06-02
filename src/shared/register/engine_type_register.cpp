#include "engine.h"
#include "prefix_shared.h"


ERef<Engine::EValueDescription> Engine::StringDescription() 
{
        static ERef<EValueDescription> result = EMakeRef<EValueDescription>(EValueType::PRIMITIVE, E_TYPEID_STRING);
        return result;

}

ERef<Engine::EValueDescription> Engine::IntegerDescription() 
{
    static ERef<EValueDescription> result = EMakeRef<EValueDescription>(EValueType::PRIMITIVE, E_TYPEID_INTEGER);
    return result;
}

ERef<Engine::EValueDescription> Engine::DoubleDescription() 
{
    static ERef<EValueDescription> result = EMakeRef<EValueDescription>(EValueType::PRIMITIVE, E_TYPEID_DOUBLE);
    return result;
}

ERef<Engine::EValueDescription> Engine::BoolDescription() 
{
    static ERef<EValueDescription> result = EMakeRef<EValueDescription>(EValueType::PRIMITIVE, E_TYPEID_BOOL);
    return result;
}



using namespace Engine;

EValueDescription::EValueDescription(EValueType type, EString id) 
    : fType(type), fID(id)
{

}

EValueDescription::~EValueDescription() 
{
    
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

EStructDescription& EStructDescription::AddField(const EString& name, ERef<EValueDescription> description) 
{
    fFields[name] = description;
    return *this;
}

const EUnorderedMap<EString, ERef<EValueDescription>>& EStructDescription::GetFields() const
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

EArrayDescription::EArrayDescription(ERef<EValueDescription> arrayType) 
    : EValueDescription(EValueType::ARRAY, arrayType->GetId() + "List"), fType(arrayType)
{
    E_ASSERT(arrayType, "ERROR: Array need type descpription!");
}

EArrayDescription::~EArrayDescription() 
{
    
}

ERef<EValueDescription> EArrayDescription::GetElementType() const
{
    return fType;
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