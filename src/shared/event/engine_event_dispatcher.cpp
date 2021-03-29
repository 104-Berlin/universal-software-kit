#include "engine.h"
#include "prefix_shared.h"

using namespace Engine;

void EEventDispatcher::Enqueue(ERef<EDataHandle> dataHandle)
{
    fPostedEvents.push_back(dataHandle);
}