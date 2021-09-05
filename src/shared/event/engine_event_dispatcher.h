#pragma once

namespace Engine {

    class E_API EEventDispatcher
    {
        struct EventData
        {
            EValueDescription::t_ID Type;
            EProperty* Data;
        };
    public:
        using CallbackFunction = std::function<void(EProperty*)>;
    private:
        EVector<EventData> fPostedEvents;
        EUnorderedMap<EValueDescription::t_ID, EVector<CallbackFunction>> fRegisteredCallbacks;
    public:
        template <typename Callback>
        auto Connect(const EValueDescription& dsc, Callback& fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn)>::value, void>
        {
            fRegisteredCallbacks[dsc.GetId()].push_back([fn](EProperty*){fn();});
        }

        template <typename Callback>
        auto Connect(const EValueDescription& dsc, Callback&& fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn), EProperty*>::value, void>
        {
            fRegisteredCallbacks[dsc.GetId()].push_back(fn);
        }

        template <typename Event, typename Callback>
        auto Connect(const EValueDescription& dsc, Callback&& fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn), Event>::value, void>
        {
            fRegisteredCallbacks[dsc.GetId()].push_back([fn](EProperty* property){
                EStructProperty* structProp = static_cast<EStructProperty*>(property);
                Event value;
                if (structProp->GetValue<Event>(value))
                {
                    fn(value);
                }
            });
        }


        template <typename Event, typename Callback>
        auto Connect(Callback&& fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn), Event>::value, void>
        {
            fRegisteredCallbacks[Event::_dsc.GetId()].push_back([fn](EProperty* property){
                EStructProperty* structProp = static_cast<EStructProperty*>(property);
                Event value;
                if (structProp->GetValue<Event>(value))
                {
                    fn(value);
                }
            });
        }

        template <typename Event, typename Callback>
        auto Connect(Callback&& fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn)>::value, void>
        {
            fRegisteredCallbacks[Event::_dsc.GetId()].push_back([fn](EProperty* property){
                fn();
            });
        }

        template <typename T>
        void Post(const EValueDescription& dsc, const T& data)
        {
            E_ASSERT_M(dsc.GetType() == EValueType::STRUCT, "Only post struct events with template!");
            EStructProperty* property = static_cast<EStructProperty*>(EProperty::CreateFromDescription(dsc.GetId(), dsc));
            if (property->SetValue<T>(data))
            {
                fPostedEvents.push_back({dsc.GetId(), property});
            }
            else
            {
                E_ERROR(EString("Property could not be initialized with given type: ") + typeid(T).name());
                delete property;
                return;
            }
        }


        template <typename T>
        void Post(const T& data)
        {
            Post<T>(T::_dsc, data);
        }

        void Post(EValueDescription dsc, EProperty* property);


        void Update();
    };

}