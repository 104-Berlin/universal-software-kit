#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

void EEventDispatcher::Enqueue_P(EValueDescription dsc, ERef<EProperty> property) 
{
    std::unique_lock<std::mutex> lock(fEventMutex);
    E_ASSERT_M(dsc == property->GetDescription(), "Description has to match the property description when posting event!");
    fPostedEvents.push_back({dsc.GetId(), property->Clone()});
    fNewEvent.notify_all();
}

void EEventDispatcher::Post_P(const EValueDescription& dsc, ERef<EProperty> property) 
{
    std::lock_guard<std::mutex> lock(fEventMutex);
    for (auto& entry : fRegisteredCallbacks)
    {
        for (CallbackFunction func : entry.second[dsc.GetId()])
        {
            func(property);
        }
    }
    for (CallbackFunction func : fCallAllways)
    {
        func(property);
    }
}

void EEventDispatcher::Update() 
{
    EVector<EventData> copiedEvents;
    {
        std::unique_lock<std::mutex> lock(fEventMutex);
        for (auto& entry : fPostedEvents)
        {
            copiedEvents.push_back({entry.Type, entry.Data});
        }
        fPostedEvents.clear();
    }
    for (auto& entry : copiedEvents)
    {
        for (auto& cbentry : fRegisteredCallbacks)
        {
            for (auto& cb : cbentry.second[entry.Type])
            {
                cb(entry.Data);
            }
        }
        
        for (auto& cb : fCallAllways)
        {
            cb(entry.Data);
        }
    }
}

void EEventDispatcher::WaitForEvent() 
{
    if (fPostedEvents.size() > 0) { return; }

    std::unique_lock<std::mutex> lk(fWaitMutex);
    fNewEvent.wait(lk);
}

void EEventDispatcher::StopWaiting() 
{
    fNewEvent.notify_all();
}

void EEventDispatcher::DisconnectEvents() 
{
    std::lock_guard<std::mutex> lock(fEventMutex);
    fRegisteredCallbacks.clear();
    fCallAllways.clear();
}
