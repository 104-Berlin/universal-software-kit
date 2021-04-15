#pragma once

namespace Engine {

#define EVENT_TYPE(Type) typeid(Type).hash_code()

    class E_API EEventDispatcher
    {
    using EventType = size_t;

        struct EventStorage
        {
            /* data */
            EventType   Type;
            void*       EventData;
            std::function<void()> CleanUp;

            EventStorage()
                : Type(0), EventData(0)
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
            static void Init(const Event& evt, EventStorage& storage)
            {
                storage.Type = EVENT_TYPE(Event);
                if (storage.EventData)
                {
                    delete ((Event*)storage.EventData);
                }
                storage.EventData = new Event();
                *((Event*)storage.EventData) = evt;
                storage.CleanUp = [&storage](){
                    if (storage.EventData)
                    {
                        delete ((Event*)storage.EventData);
                    }
                };
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
            fPostedEvents.push_back(EventStorage());  
            EventStorage::Init<Event>(evt, fPostedEvents.back());
        }

        template <typename Event>
        void Post(const Event& evt)
        {
            for (std::function<void(void*)> fn : fObervers[EVENT_TYPE(Event)])
            {
                fn(&evt);
            }
        }

        void Update()
        {
            for (EventStorage& entry : fPostedEvents)
            {
                for (std::function<void(void*)> fn : fObervers[entry.Type])
                {                    
                    fn(entry.EventData);
                }
                entry.CleanUp();
            }
            fPostedEvents.clear();
        }

        void CleanUp()
        {
            fObervers.clear();
            for (EventStorage& storage : fPostedEvents)
            {
                storage.CleanUp();
            }
            fPostedEvents.clear();
        }
    private:
        EUnorderedMap<EventType, EVector<std::function<void(void*)>>>   fObervers;
        EVector<EventStorage>                                           fPostedEvents;
    };

}