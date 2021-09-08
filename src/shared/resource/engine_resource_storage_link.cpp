#include "engine.h"
#include "prefix_shared.h"

#ifndef EWIN
template <>
#endif
Engine::EValueDescription Engine::getdsc::GetDescription<Engine::EResourceLink>()
{
	return Engine::EValueDescription::CreateStruct("EResourceLink", {
        {"Type", StringDescription},
        {"ResourceID", UnsignedBigIntegerDescription}
    });
}


template <>
bool convert::setter<Engine::EResourceLink>(Engine::EStructProperty* property, const Engine::EResourceLink& resourceLink)
{
    using namespace Engine;
	EValueProperty<EString>* type = (EValueProperty<EString>*)property->GetProperty("Type");
	EValueProperty<u64>* id = (EValueProperty<u64>*)property->GetProperty("ResourceID");
	if (id)
	{
		id->SetValue(resourceLink.ResourceId);
        if (type)
        {
            type->SetValue(resourceLink.Type);
        }
		return true;
	}
	return false;
}

template <>
bool convert::getter<Engine::EResourceLink>(const Engine::EStructProperty* property, Engine::EResourceLink* outLink)
{
    using namespace Engine;
	const EValueProperty<EString>* type = (EValueProperty<EString>*)property->GetProperty("Type");
	const EValueProperty<u64>* id = (EValueProperty<u64>*)property->GetProperty("ResourceID");
	if (id)
	{
		outLink->ResourceId = id->GetValue();
        if (type)
        {
            outLink->Type = type->GetValue();
        }
		return true;
	}
	return false;
}