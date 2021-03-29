#pragma once

namespace Engine {

    class E_API EEventDispatcher
    {
    public:
        void Enqueue(ERef<EDataHandle> dataHandle);
        void Update();
    private:
        EUnorderedMap<EString, EVector<std::function<void(ERef<EDataHandle>)>>>  fObservers;
        EVector<ERef<EDataHandle>>                                               fPostedEvents;
    };

}