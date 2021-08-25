#pragma once

namespace Engine {

    class E_API EEventDispatcher
    {
    public:
        using CallbackFunction = std::function<void(EProperty*)>;
    private:
        EUnorderedMap<EValueDescription::t_ID, EProperty*> fComponentStorage;
        EUnorderedMap<EValueDescription::t_ID, CallbackFunction> fRegisteredCallbacks;
    public:
        template <typename Callback>
        auto Connect(const EValueDescription& dsc, Callback& fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn)>::value, void>
        {
            fRegisteredCallbacks.insert({dsc.GetId(), [&fn](EProperty*){fn()}});
        }

        template <typename Callback>
        auto Connect(const EValueDescription& dsc, Callback&& fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn), EProperty*>::value, void>
        {
            fRegisteredCallbacks.insert({dsc.GetId(), fn});
        }

        template <typename Event, typename Callback>
        auto Connect(const EValueDescription& dsc, Callback&& fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn), Event&>::value, void>
        {
            fRegisteredCallbacks.insert({dsc.GetId(), [&fn](EProperty* property){
                EStructProperty* structProp = static_cast<EStructProperty*>(property);
                fn(structProp->GetValue<Event>());
            }});
        }

        template <typename T>
        void Post(const EValueDescription& dsc, const T& data)
        {
            E_ASSERT(dsc.fType == EValueType::STRUCT, "Only post struct events with template!");
            EStructProperty* property = static_cast<EStructProperty*>(EProperty::CreateFromDescription(dsc));
            if (property->SetValue<T>(data))
            {
                fComponentStorage.insert({dsc.GetId(), property});
            }
            else
            {
                E_ERROR("Property could not be initialized with given type: " + typeid(T).name());
                delete property;
                return;
            }
        }

        void Post(EValueDescription dsc, EProperty* property);
    };

}