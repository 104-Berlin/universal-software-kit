#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

void EEventDispatcher::Post(EValueDescription dsc, EProperty* property) 
{
    E_ASSERT(dsc == property->GetDescription(), "Description has to match the property description when posting event!");
    fPostedEvents.push_back({dsc.GetId(), property->Clone()});
}

void EEventDispatcher::Update() 
{
    for (auto& entry : fPostedEvents)
    {
        for (auto& cb : fRegisteredCallbacks[entry.Type])
        {
            cb(entry.Data);
        }
        delete entry.Data;
    }
    fPostedEvents.clear();
}
