#pragma once

namespace Engine {

    namespace shared {

        class E_INTER_API ERegisterEventDispatcher : public EEventDispatcher
        {
        public:
            template <typename Callback>
            void AddComponentCreateEventListener(const EValueDescription& description, Callback cb, void* key = 0)
            {
                Connect<ComponentCreateEvent>([cb, description](ComponentCreateEvent event){
                    if (event.ValueId == description.GetId())
                    {
                        std::invoke(cb, event.Handle);
                    }
                }, key);
            }

            template <typename Callback>
            void AddComponentDeleteEventListener(const EValueDescription& description, Callback cb, void* key = 0)
            {
                Connect<ComponentDeleteEvent>([cb, description](ComponentDeleteEvent event){
                    if (description.GetId() == event.ValueId)
                    {
                        std::invoke(cb, event.Handle);
                    }
                }, key);
            }

            template <typename Callback>
            void AddEntityChangeEventListener(const EString& valueIdent, Callback cb, void* key = 0)
            {
                Connect<ValueChangeEvent>([cb, valueIdent](ValueChangeEvent event){
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
            EExtensionManager               fExtensionManager;   // Loaded extensions, e.G. Components, ResourceTypes, etc.
            ERegisterConnection             fRegisterConnection; // Client communication with Register
            ERegisterSocket*                fRegisterSocket;     // Register Provider / Server (Entities and Data)
            ERegisterEventDispatcher        fRegisterEventDispatcher; // Event Dispatcher for Register. All events the register provides are dispatched here.
        public:
            StaticSharedContext();
            ~StaticSharedContext();

            EExtensionManager&  GetExtensionManager();
            ERegisterConnection&  GetRegisterConnection();

            ERegisterEventDispatcher& Events();

            bool IsLocaleServerRunning() const;
            void RestartLocaleServer(int port = 1420);

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
                StaticSharedContext::instance().Events().Disconnect(this);
            }
        };

        E_INTER_API ERegisterEventDispatcher& Events();
        E_INTER_API EExtensionManager& ExtensionManager();

        
        // ERROR:
        // false = NO ERROR
        // true = ERROR!
        using ESharedError = bool; // Error is bool for now. Replace with actual errors


        // Setter
        ESharedError E_INTER_API LoadExtension(const EString& pathToExtension);
        ESharedError E_INTER_API CreateEntity();

        ESharedError E_INTER_API LoadRegisterFromBuffer(ESharedBuffer buffer);
        
        ESharedError E_INTER_API CreateComponent(const EString& componentId, EDataBase::Entity entity);
        ESharedError E_INTER_API CreateComponent(const EValueDescription& componentId, EDataBase::Entity entity);
        ESharedError E_INTER_API CreateComponent(EStructProperty* componentValue, EDataBase::Entity entity);

        template <typename T>
        ESharedError CreateComponent(EDataBase::Entity entity)
        {
            return CreateComponent(getdsc::GetDescription<T>(), entity);
        }


        ESharedError E_INTER_API CreateResource(EResourceData* data);

        ESharedError E_INTER_API SetValue(EDataBase::Entity entity, const EString& valueIdent, const EString& valueString);
        ESharedError E_INTER_API SetValue(EDataBase::Entity entity, const EString& valueIdent, EProperty* value);

        template <typename T>
        ESharedError SetValue(EDataBase::Entity entity, const EString& valueIdent, const T& value)
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

        ESharedError E_INTER_API SetEnumValue(EDataBase::Entity entity, const EString& valueIdent, u32 value);
        ESharedError E_INTER_API AddArrayEntry(EDataBase::Entity entity, const EString& ident);


        // Getter
        E_INTER_API EVector<EDataBase::Entity> GetAllEntites();
        
        E_INTER_API ERef<EProperty> GetValueFromIdent(EDataBase::Entity entity, const EString& vlaueIdent);

        template <typename T>
        bool GetValue(EDataBase::Entity entity, T* value)
        {
            EValueDescription dsc = getdsc::GetDescription<T>();
            if (dsc.Valid())
            {
                ERef<EProperty> foundProp = GetValueFromIdent(entity, dsc.GetId());
                if (foundProp)
                {
                    if (convert::getter(foundProp.get(), value))
                    {
                        return true;
                    }
                }
            }
            return false;
        }

       E_INTER_API EVector<ERef<EProperty>> GetAllComponents(EDataBase::Entity entity);
       E_INTER_API ERef<EResourceData> GetResource(EResourceData::t_ID id);
       E_INTER_API EVector<ERef<EResourceData>> GetLoadedResource(const EString& resourceType = ""); // This wont return the data of the resource. Fetch them manuel
       E_INTER_API ESharedBuffer GetRegisterAsBuffer();
    }

}
