#pragma once

namespace Engine {

    class E_API EResourceManager
    {
    private:
        EUnorderedMap<size_t, EMap<EString, ESharedBuffer>> fLoadedResources;
    public:
        EResourceManager();

        template <typename ResourceType>
        void AddOrUpdateResource(const EString& identifier, const ResourceType& resourceData)
        {
            EMap<EString, ESharedBuffer>& typesLoadedResources = fLoadedResources[typeid(ResourceType).hash_code()];
            if (typesLoadedResources.find(identifier) == typesLoadedResources.end())
            {
                ESharedBuffer sb;
                sb.InitWith<ResourceType>(&resourceData, sizeof(ResourceType));
                typesLoadedResources.insert({identifier, sb});
            }
        }
    };

}