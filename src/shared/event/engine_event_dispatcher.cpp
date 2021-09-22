#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

void EEventDispatcher::Enqueue_P(EValueDescription dsc, EProperty* property) 
{
    std::unique_lock<std::mutex> lock(fEventMutex);
    E_ASSERT_M(dsc == property->GetDescription(), "Description has to match the property description when posting event!");
    fPostedEvents.push_back({dsc.GetId(), property->Clone()});
    lock.unlock();
    fNewEvent.notify_all();
}

void EEventDispatcher::Post_P(const EValueDescription& dsc, EProperty* property) 
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
    std::unique_lock<std::mutex> lock(fEventMutex);
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

void EEventDispatcher::WaitForEvent() 
{
    if (fPostedEvents.size() > 0) { return; }

    std::unique_lock<std::mutex> lk(fWaitMutex);
    fNewEvent.wait(lk, [this]{return fPostedEvents.size() > 0;});
}

void EEventDispatcher::DisconnectEvents() 
{
    std::lock_guard<std::mutex> lock(fEventMutex);
    fRegisteredCallbacks.clear();
    fCallAllways.clear();
}
