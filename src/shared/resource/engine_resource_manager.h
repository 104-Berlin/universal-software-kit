#pragma once

namespace Engine {

    class E_API EResourceManager
    {
    private:
        EUnorderedMap<size_t, EUnorderedMap<EString, ESharedBuffer>> fLoadedResources;
    public:
        EResourceManager();

        template <typename ResourceType>
        void AddOrUpdateResource(const EString& identifier, const ResourceType& resourceData)
        {
            EUnorderedMap<EString, ESharedBuffer>& typesLoadedResources = fLoadedResources[typeid(ResourceType).hash_code()];
            if (typesLoadedResources.find(identifier) == typesLoadedResources.end())
            {
                ESharedBuffer sb;
                sb.InitWith<ResourceType>(&resourceData, sizeof(ResourceType));
                typesLoadedResources[identifier] = sb;
            }
        }

        template <typename ResourceType>
        const ResourceType* GetResource(const EString& identifier)
        {
            const size_t typeId = typeid(ResourceType).hash_code();
            const ESharedBuffer resultData = fLoadedResources[typeid(ResourceType).hash_code()][identifier];
            if (!resultData.IsNull())
            {
                return resultData.Data<ResourceType>();
            }
            return nullptr;
        }
    };

}