#pragma once

namespace Engine {

    class EComponentPanel;

    class E_API EScene
    {
        using ObjectCallback = std::function<void(EObject)>;
    private:
        EProperty<EString>                              fName;
        //EObjectRef                                      fSelectedObject;

        //EResourceManager                                fResourceManager;


        // Entities
        entt::registry                                  fRegistry;
        EUnorderedMap<EUUID, EObject, UuidHash>         fEntityMap;
    public:
        EScene(const EString& name);
        ~EScene();


        //EResourceManager&   GetResourceManager();


        void Update(float delta);


        EObject CreateObject();
        EObject GetObjectByUuid(const EUUID& uuid);
        //EObjectRef& GetSelectedObject();

        void SetJsObject(EJson& json);
        void FromJsObject(const EJson& json);

        // Looping functions
        void Clear();
        void ForEachObject(ObjectCallback fn);


        template <typename ...Component>
        auto view()
        {
            return fRegistry.view<Component...>();
        }
    public:
        friend class EObject;
    private:
        void CallObjectFunc(EEntity entity, ObjectCallback fn);
        void LoadDefaultMeshes();
    };

}