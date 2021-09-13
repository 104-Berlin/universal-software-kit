#pragma once

namespace Engine {

    namespace shared {

        class E_INTER_API StaticSharedContext
        {
        private:
            EExtensionManager               fExtensionManager;
            std::thread                     fRunningThread;
            EQueue<std::function<void()>>   fMainThreadQueue;
            std::mutex                      fQueueMutex;
            std::atomic<bool>               fIsRunning;
        public:
            StaticSharedContext();
            ~StaticSharedContext();

            EExtensionManager& GetExtensionManager();

            static void RunInMainThread(std::function<void()> function);

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

        ESharedError E_INTER_API LoadExtension(const EString& pathToExtension);
        ESharedError E_INTER_API CreateEntity();

        ESharedError E_INTER_API CreateComponent(const EString& componentId, ERegister::Entity entity);
    }

}