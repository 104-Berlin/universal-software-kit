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
        EVector<CallbackFunction> fCallAllways;

        std::mutex  fEventMutex;

        std::mutex              fWaitMutex;
        std::condition_variable fNewEvent;
    public:
        ~EEventDispatcher()
        {
            fRegisteredCallbacks.clear();
            for (EventData data : fPostedEvents)
            {
                delete data.Data;
            }
            fPostedEvents.clear();
        }

        template <typename Callback>
        auto Connect(const EValueDescription& dsc, Callback fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn)>::value, EVector<CallbackFunction>::iterator>
        {
            std::lock_guard<std::mutex> lock(fEventMutex);
            fRegisteredCallbacks[dsc.GetId()].push_back([fn](EProperty*){fn();});
            fRegisteredCallbacks[dsc.GetId()].back();
        }

        template <typename Callback>
        auto Connect(const EValueDescription& dsc, Callback fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn), EProperty*>::value, EVector<CallbackFunction>::iterator>
        {
            std::lock_guard<std::mutex> lock(fEventMutex);
            fRegisteredCallbacks[dsc.GetId()].push_back(fn);
            fRegisteredCallbacks[dsc.GetId()].back();
        }

        template <typename Event, typename Callback>
        auto Connect(const EValueDescription& dsc, Callback fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn), Event>::value, EVector<CallbackFunction>::iterator>
        {
            std::lock_guard<std::mutex> lock(fEventMutex);
            EValueDescription::t_ID valueId = getdsc::GetDescription<Event>().GetId();
            fRegisteredCallbacks[valueId].push_back([fn](EProperty* property){
                EStructProperty* structProp = static_cast<EStructProperty*>(property);
                Event value;
                if (structProp->GetValue<Event>(value))
                {
                    fn(value);
                }
            });
            return fRegisteredCallbacks[valueId].back();
        }


        template <typename Event, typename Callback>
        auto Connect(Callback fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn), Event>::value, EVector<CallbackFunction>::iterator>
        {
            std::lock_guard<std::mutex> lock(fEventMutex);
            EValueDescription::t_ID valueId = getdsc::GetDescription<Event>().GetId();
            fRegisteredCallbacks[valueId].push_back([fn](EProperty* property){
                EStructProperty* structProp = static_cast<EStructProperty*>(property);
                Event value;
                if (structProp->GetValue<Event>(value))
                {
                    fn(value);
                }
            });
            fRegisteredCallbacks[valueId].back();
        }

        template <typename Event, typename Callback>
        auto Connect(Callback fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn)>::value, EVector<CallbackFunction>::iterator>
        {
            std::lock_guard<std::mutex> lock(fEventMutex);
            EValueDescription::t_ID valueId = getdsc::GetDescription<Event>().GetId();
            fRegisteredCallbacks[valueId].push_back([fn](EProperty* property){
                fn();
            });
            return fRegisteredCallbacks[valueId].back();
        }

        template <typename Callback>
        auto ConnectAll(Callback fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn), EStructProperty*>::value, EVector<CallbackFunction>::iterator>
        {
            std::unique_lock<std::mutex> lock(fEventMutex);
            fCallAllways.push_back([fn](EProperty* prop){fn(static_cast<EStructProperty*>(prop)); });
            return fCallAllways.back();
        }

        void Enqueue_P(EValueDescription dsc, EProperty* property);

        template <typename T>
        void Enqueue(const EValueDescription& dsc, const T& data)
        {
            E_ASSERT_M(dsc.GetType() == EValueType::STRUCT, "Only post struct events with template!");
            EStructProperty* property = static_cast<EStructProperty*>(EProperty::CreateFromDescription(dsc.GetId(), dsc));
            if (property->SetValue<T>(data))
            {
                Enqueue_P(dsc, property);
            }
            else
            {
                E_ERROR(EString("Property could not be initialized with given type: ") + typeid(T).name());
                delete property;
                return;
            }
        }


        template <typename T>
        void Enqueue(const T& data)
        {
            Enqueue<T>(getdsc::GetDescription<T>(), data);
        }

        void Post_P(const EValueDescription& dsc, EProperty* property);

        template <typename T>
        void Post(const EValueDescription& dsc, const T& data)
        {
            E_ASSERT_M(dsc.GetType() == EValueType::STRUCT, "Only post struct events with template!");
            EStructProperty* property = static_cast<EStructProperty*>(EProperty::CreateFromDescription(dsc.GetId(), dsc));
            if (property->SetValue<T>(data))
            {
                Post_P(dsc, property);
            }
            delete property;
        }

        template <typename T>
        void Post(const T& data)
        {
            Post<T>(getdsc::GetDescription<T>(), data);
        }


        void Update();
        void WaitForEvent();
        void StopWaiting();

        void DisconnectEvents();
    };

}