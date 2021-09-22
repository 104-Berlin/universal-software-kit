#pragma once

namespace Engine {

    namespace shared {

        class E_INTER_API StaticSharedContext
        {
        private:
            EExtensionManager               fExtensionManager;
            ERegisterConnection             fRegisterConnection;
            ERegisterSocket*                fRegisterSocket;
        public:
            StaticSharedContext();
            ~StaticSharedContext();

            EExtensionManager&  GetExtensionManager();
            ERegisterConnection&  GetRegisterConnection();

            void ConnectTo(const EString& address);

            static void Start();
            static void Stop();

            static StaticSharedContext& instance();
        private:
            static StaticSharedContext* fInstance;
        };
        
        // ERROR:
        // false = NO ERROR
        // true = ERROR!
        using ESharedError = bool; // Error is bool for now. Replace with actual errors


        // Setter
        ESharedError E_INTER_API LoadExtension(const EString& pathToExtension);
        ESharedError E_INTER_API CreateEntity();

        ESharedError E_INTER_API CreateComponent(const EString& componentId, ERegister::Entity entity);

        ESharedError E_INTER_API SetValue(ERegister::Entity entity, const EString& valueIdent, const EString& valueString);


        // Getter
        
       E_INTER_API ERef<EProperty> GetValue(ERegister::Entity entity, const EString& vlaueIdent);
    }

}
