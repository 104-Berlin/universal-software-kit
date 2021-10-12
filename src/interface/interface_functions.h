#pragma once

namespace Engine {

    namespace shared {

        class E_INTER_API ERegisterEventDispatcher
        {
        private:
            EEventDispatcher fEventDispatcher;

        public:
            EEventDispatcher& GetEventDispatcher();
            const EEventDispatcher& GetEventDispatcher() const;

            template <typename Callback>
            void AddComponentCreateEventListener(const EValueDescription& description, Callback cb, void* key = 0)
            {
                fEventDispatcher.Connect<ComponentCreateEvent>([cb, description](ComponentCreateEvent event){
                    if (event.ValueId == description.GetId())
                    {
                        cb(event.Handle);
                    }
                }, key);
            }

            template <typename Callback>
            void AddComponentDeleteEventListener(const EValueDescription& description, Callback cb, void* key = 0)
            {
                fEventDispatcher.Connect<ComponentDeleteEvent>([cb, description](ComponentDeleteEvent event){
                    if (description.GetId() == event.ValueId)
                    {
                        std::invoke(cb, event.Handle);
                    }
                }, key);
            }

            template <typename Callback>
            void AddEntityChangeEventListener(const EString& valueIdent, Callback cb, void* key = 0)
            {
                fEventDispatcher.Connect<ValueChangeEvent>([cb, valueIdent](ValueChangeEvent event){
                    if (event.Identifier.length() < valueIdent.length()) {return;}
                    if (valueIdent == event.Identifier.substr(0, valueIdent.length()))
                    {
                        std::invoke(cb, event.Handle, valueIdent);
                    }
                }, key);
            }
        };


        class E_INTER_API StaticSharedContext
        {
        private:
            EExtensionManager               fExtensionManager;
            ERegisterConnection             fRegisterConnection;
            ERegisterSocket*                fRegisterSocket;
            ERegisterEventDispatcher        fRegisterEventDispatcher;
        public:
            StaticSharedContext();
            ~StaticSharedContext();

            EExtensionManager&  GetExtensionManager();
            ERegisterConnection&  GetRegisterConnection();

            ERegisterEventDispatcher& Events();

            void ConnectTo(const EString& address);

            static void Start();
            static void Stop();

            static StaticSharedContext& instance();
        private:
            static StaticSharedContext* fInstance;
        };


        class EAutoContextEventRelease
        {
        public:
            virtual ~EAutoContextEventRelease()
            {
                StaticSharedContext::instance().Events().GetEventDispatcher().Disconnect(this);
            }
        };
        
        // ERROR:
        // false = NO ERROR
        // true = ERROR!
        using ESharedError = bool; // Error is bool for now. Replace with actual errors


        
        extern "C" ESharedError E_INTER_API CreateComponent_c_str(const char* componentId, ERegister::Entity entity);


            // Setter
            ESharedError E_INTER_API LoadExtension(const EString& pathToExtension);
            ESharedError E_INTER_API CreateEntity();

            ESharedError E_INTER_API LoadRegisterFromBuffer(ESharedBuffer buffer);
            
            ESharedError E_INTER_API CreateComponent(const EString& componentId, ERegister::Entity entity);
            ESharedError E_INTER_API CreateComponent(const EValueDescription& componentId, ERegister::Entity entity);
            ESharedError E_INTER_API CreateResource(EResourceData* data);

            ESharedError E_INTER_API SetValue(ERegister::Entity entity, const EString& valueIdent, const EString& valueString);


            ESharedError E_INTER_API AddArrayEntry(ERegister::Entity entity, const EString& ident);


            // Getter
            
            E_INTER_API ERef<EProperty> GetValue(ERegister::Entity entity, const EString& vlaueIdent);

                

            E_INTER_API EVector<ERef<EProperty>> GetAllComponents(ERegister::Entity entity);
            E_INTER_API ERef<EResourceData> GetResource(EResourceData::t_ID id);
            E_INTER_API EVector<ERef<EResourceData>> GetLoadedResource(); // This wont return the data of the resource. Fetch them manuel
            E_INTER_API ESharedBuffer GetRegisterAsBuffer();


       // Templates for easy c access

        template <typename T>
        ESharedError SetValue(ERegister::Entity entity, const EString& valueIdent, const T& value)
        {
            EValueDescription dsc = getdsc::GetDescription<T>();
            if (dsc.Valid())
            {
                EProperty* property = EProperty::CreateFromDescription(dsc.GetId(), dsc);
                convert::setter(property, value);
                EString propertyValue = ESerializer::WritePropertyToJs(property).dump();
                SetValue(entity, valueIdent, propertyValue);
            }
            return false;
        }

        template <typename T>
        T GetValue(ERegister::Entity entity)
        {
            T result;
            EValueDescription dsc = getdsc::GetDescription<T>();
            if (dsc.Valid())
            {
                ERef<EProperty> foundProp = GetValue(entity, dsc.GetId());
                if (foundProp)
                {
                    convert::getter(foundProp.get(), &result);
                }
            }
            return result;
        }
    }

}
