#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

void EEventDispatcher::Post(EValueDescription dsc, EProperty* property) 
{
    E_ASSERT(dsc == property->GetDescription(), "Description has to match the property description when posting event!");
    fComponentStorage.insert({dsc.GetId(), property->Clone()});
}
