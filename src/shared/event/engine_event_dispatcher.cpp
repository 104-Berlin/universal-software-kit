#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

void EEventDispatcher::Enqueue(ERef<EDataHandle> dataHandle)
{
    fPostedEvents.push_back(dataHandle);
}

void EEventDispatcher::Update() 
{
    for (ERef<EDataHandle> eventData : fPostedEvents)
    {
        for (std::function<void(ERef<EDataHandle>)> fn : fObservers[eventData->GetTypeName()])
        {
            fn(eventData);
        }
    }
    fPostedEvents.clear();
}