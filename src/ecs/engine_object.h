#pragma once

namespace Engine {

    class EScene;

    class E_API EObject
    {
    private:
        EScene*             fScene = nullptr;
        EEntity             fHandle = entt::null;
    public:
        EObject() = default;
        EObject(EEntity handle, EScene* scene);
        EObject(const EObject& other)
            : fScene(other.fScene), fHandle(other.fHandle)
        {}
        virtual ~EObject() = default;

        // Wrapper functions
        template <typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            T& newComponent = fScene->fRegistry.emplace<T>(fHandle, std::forward<Args>(args)...);
            // Set the scene for all the properties inside the component
            newComponent._reflect([this](const char*, auto property){
                property->SetScene(fScene);
            });
            return newComponent;
        }

        template <typename T>
        bool HasComponent()
        {
            return fScene->fRegistry.has<T>(fHandle);
        }

        template <typename T>
        T& GetComponent()
        {
            return fScene->fRegistry.get<T>(fHandle);   
        }

        template <typename T>
        const T& GetComponent() const
        {
            return fScene->fRegistry.get<T>(fHandle);   
        }

        EEntity GetHandle() const
        {
            return fHandle;
        }

        EUUID GetUuid() const;
        
        void Delete()
        {
            fScene->fRegistry.destroy(fHandle);
        }       

        operator bool () const
        {
            return fHandle != entt::null && fScene;
        }

        bool operator==(const EObject& other)
        {
            return fScene == other.fScene && fHandle == other.fHandle;
        }

        bool operator!=(const EObject& other)
        {
            return !((*this) == other);
        }

        void Set(EEntity entity, EScene* scene);
        void Set(EObject object);

        void FromJsObject(const EJson& ref);
        void SetJsObject(EJson& ref) const;  

        virtual void OnFromJsObject(const EJson& ref);
        virtual void OnSetJsObject(EJson& ref) const;
    };
}
