#pragma once

namespace Engine {

    class E_API EEventDispatcher
    {
    public:

        template<typename CallbackFn>
        auto Connect(const EStructureDescription& eventDataDsc, CallbackFn&& fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn), ERef<EDataHandle>>::value, void>
        {
            fObservers[eventDataDsc.GetDataName()].push_back([fn](ERef<EDataHandle> val){
                fn(val);
            });
        }

        template<typename CallbackFn>
        auto Connect(const EStructureDescription& eventDataDsc, CallbackFn&& fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn)>::value, void>
        {
            fObservers[eventDataDsc.GetDataName()].push_back([fn](ERef<EDataHandle>){
                fn();
            });
        }

        void Enqueue(ERef<EDataHandle> dataHandle);
        void Update();
    private:
        EUnorderedMap<EString, EVector<std::function<void(ERef<EDataHandle>)>>>  fObservers;
        EVector<ERef<EDataHandle>>                                               fPostedEvents;
    };

}