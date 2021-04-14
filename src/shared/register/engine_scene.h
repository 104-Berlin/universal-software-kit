#pragma once

namespace Engine {

    class E_API EScene
    {
    private:
        friend class EObject;
        entt::registry fRegistry;
    public:
        EScene();
        ~EScene();

        void AddObject();
    };

    class E_API EObject
    {
    private:
        entt::entity    fHandle;
        EScene*         fScene;
    public:
        EObject(EScene* scene, entt::entity handle);

        template <typename T, typename ... Args>
        T& AddComponent(Args&&... args)
        {
            E_ASSERT(fScene, "Invalid scene set for object. Cant add Component!");
            E_ASSERT(fHandle != entt::null, "Invalid entity. Cant add Component!");
            fScene->fRegistry.emplace<T>(fHandle, std::forward<Args>(args)...);
        }

        template <typename T>
        bool HasComponent() const
        {
            //fScene->fRegistry.has<T>(fHandle);
        }
    };

}