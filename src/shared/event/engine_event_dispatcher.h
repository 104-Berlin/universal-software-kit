#pragma once

namespace Engine {

#define EVENT_TYPE(Type) typeid(Type).hash_code()

    class E_API EEventDispatcher
    {
    using EventType = size_t;

        struct EventStorage
        {
            /* data */
            EventType       Type;
            ESharedBuffer   EventData;

            EventStorage()
                : Type(0), EventData()
            {}

            ~EventStorage()
            {}

            EventStorage(const EventStorage& storage)
            {
                Type = storage.Type;
                EventData = storage.EventData;
            }

            void operator=(const EventStorage& other)
            {
                Type = other.Type;
                EventData = other.EventData;
            }

            template <typename Event>
            void Init(const Event& evt)
            {
                Type = EVENT_TYPE(Event);
                EventData.InitWith<Event>(&evt, sizeof(Event));
            }

        };
        
    public:

        template<typename Event, typename CallbackFn>
        auto Connect(CallbackFn&& fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn), Event&>::value, void>
        {
            fObervers[EVENT_TYPE(Event)].push_back([fn](void* evtData){
                fn(*((Event*)evtData));
            });
        }

        template<typename Event, typename CallbackFn>
        auto Connect(CallbackFn&& fn)
        -> std::enable_if_t<std::is_invocable<decltype(fn)>::value, void>
        {
            fObervers[EVENT_TYPE(Event)].push_back([fn](void* evtData){
                fn();
            });
        }

        template <typename Event>
        void Enqueue(const Event& evt)
        {
            EventStorage storage;
            storage.Init<Event>(evt);
            fPostedEvents.push_back(storage);  
        }

        template <typename Event>
        void Post(const Event& evt)
        {
            for (std::function<void(void*)> fn : fObervers[EVENT_TYPE(Event)])
            {
                fn((void*)&evt);
            }
        }

        void Update()
        {
            for (EventStorage& entry : fPostedEvents)
            {
                for (std::function<void(void*)> fn : fObervers[entry.Type])
                {                    
                    fn(entry.EventData.Data());
                }
            }
            fPostedEvents.clear();
        }

        void CleanUp()
        {
            fObervers.clear();
            fPostedEvents.clear();
        }
    private:
        EUnorderedMap<EventType, EVector<std::function<void(void*)>>>   fObervers;
        EVector<EventStorage>                                           fPostedEvents;
    };

}