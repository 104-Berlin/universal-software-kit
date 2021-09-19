#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

void EEventDispatcher::Enqueue(EValueDescription dsc, EProperty* property) 
{
    std::lock_guard<std::mutex> lock(fEventMutex);
    E_ASSERT_M(dsc == property->GetDescription(), "Description has to match the property description when posting event!");
    fPostedEvents.push_back({dsc.GetId(), property->Clone()});
}

void EEventDispatcher::Post(const EValueDescription& dsc, EProperty* property) 
{
    std::lock_guard<std::mutex> lock(fEventMutex);
    for (CallbackFunction func : fRegisteredCallbacks[dsc.GetId()])
    {
        func(property);
    }
    for (CallbackFunction func : fCallAllways)
    {
        func(property);
    }
}

void EEventDispatcher::Update() 
{
    std::lock_guard<std::mutex> lock(fEventMutex);
    for (auto& entry : fPostedEvents)
    {
        for (auto& cb : fRegisteredCallbacks[entry.Type])
        {
            cb(entry.Data);
        }
        for (auto& cb : fCallAllways)
        {
            cb(entry.Data);
        }
        delete entry.Data;
    }
    fPostedEvents.clear();
}

void EEventDispatcher::DisconnectEvents() 
{
    std::lock_guard<std::mutex> lock(fEventMutex);
    fRegisteredCallbacks.clear();
    fCallAllways.clear();
}
