#pragma once

namespace Engine {

    class E_API EEventDispatcher
    {
        struct EventData
        {
            EValueDescription::t_ID Type;
            ERef<EProperty> Data;
        };
    public:
        using CallbackFunction = std::function<void(ERef<EProperty>)>;
    private:
        EVector<EventData> fPostedEvents;
        EUnorderedMap<void*, EUnorderedMap<EValueDescription::t_ID, EVector<CallbackFunction>>> fRegisteredCallbacks;
        EVector<CallbackFunction> fCallAllways;

        std::mutex  fEventMutex;

        std::mutex              fWaitMutex;
        std::condition_variable fNewEvent;
    public:
        ~EEventDispatcher()
        {
            fRegisteredCallbacks.clear();
            fPostedEvents.clear();
        }

        template <typename Callback>
        auto Connect(const EValueDescription& dsc, Callback fn, void* key = 0)
        -> std::enable_if_t<std::is_invocable<decltype(fn)>::value, void>
        {
            std::lock_guard<std::mutex> lock(fEventMutex);
            fRegisteredCallbacks[key][dsc.GetId()].push_back([fn](EProperty*){fn();});
        }

        template <typename Callback>
        auto Connect(const EValueDescription& dsc, Callback fn, void* key = 0)
        -> std::enable_if_t<std::is_invocable<decltype(fn), EProperty*>::value, void>
        {
            std::lock_guard<std::mutex> lock(fEventMutex);
            fRegisteredCallbacks[key][dsc.GetId()].push_back(fn);
        }

        template <typename Event, typename Callback>
        auto Connect(Callback fn, void* key = 0)
        -> std::enable_if_t<std::is_invocable<decltype(fn), Event>::value, void>
        {
            std::lock_guard<std::mutex> lock(fEventMutex);
            fRegisteredCallbacks[key][Event::_dsc.GetId()].push_back([fn](ERef<EProperty> property){
                Event value;
                if (convert::getter(property.get(), &value))
                {
                    fn(value);
                }
            });
        }

        template <typename Event, typename Callback>
        auto Connect(Callback fn, void* key = 0)
        -> std::enable_if_t<std::is_invocable<decltype(fn)>::value, void>
        {
            std::lock_guard<std::mutex> lock(fEventMutex);
            EValueDescription::t_ID valueId = getdsc::GetDescription<Event>().GetId();
            fRegisteredCallbacks[key][valueId].push_back([fn](EProperty* property){
                fn();
            });
        }

        void Disconnect(void* key)
        {
            fRegisteredCallbacks[key].clear();
        }

        template <typename Callback>
        auto ConnectAll(Callback fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn), ERef<EProperty>>::value, void>
        {
            std::unique_lock<std::mutex> lock(fEventMutex);
            fCallAllways.push_back(fn);
        }

        void Enqueue_P(EValueDescription dsc, ERef<EProperty> property);

        template <typename T>
        void Enqueue(const EValueDescription& dsc, const T& data)
        {
            E_ASSERT_M(dsc.GetType() == EValueType::STRUCT, "Only post struct events with template!");
            ERef<EStructProperty> property = std::static_pointer_cast<EStructProperty>(EProperty::CreateFromDescription(dsc.GetId(), dsc));
            if (property->SetValue<T>(data))
            {
                Enqueue_P(dsc, property);
            }
            else
            {
                E_ERROR(EString("Property could not be initialized with given type: ") + typeid(T).name());
                return;
            }
        }


        template <typename T>
        void Enqueue(const T& data)
        {
            Enqueue<T>(getdsc::GetDescription<T>(), data);
        }

        void Post_P(const EValueDescription& dsc, ERef<EProperty> property);

        template <typename T>
        void Post(const EValueDescription& dsc, const T& data)
        {
            E_ASSERT_M(dsc.GetType() == EValueType::STRUCT, "Only post struct events with template!");
            ERef<EStructProperty> property = std::dynamic_pointer_cast<EStructProperty>(EProperty::CreateFromDescription(dsc.GetId(), dsc));
            if (property->SetValue<T>(data))
            {
                Post_P(dsc, property);
            }
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