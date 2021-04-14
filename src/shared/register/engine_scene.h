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
            return fScene->fRegistry.emplace<T>(fHandle, std::forward<Args>(args)...);
        }

        template <typename T>
        bool HasComponent() const
        {
            E_ASSERT(fScene, "Invalid scene set for object. Cant check Component Existance!");
            E_ASSERT(fHandle != entt::null, "Invalid entity. Cant check Component Existance!");
            fScene->fRegistry.any_of<T>(fHandle);
        }

        template <typename T>
        T& GetComponent()
        {
            E_ASSERT(fScene, "Invalid scene set for object. Cant get Component!");
            E_ASSERT(fHandle != entt::null, "Invalid entity. Cant get Component!");
            return fScene->fRegistry.get<T>(fHandle);
        }

        static EObject Create(EScene* scene);
        static void Delete(EObject object);
    };

}