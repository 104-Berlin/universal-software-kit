#pragma once

namespace Engine {

    template <typename T>
    class EExtensionRegister
    {
    protected:
        EUnorderedMap<EString, EVector<T>> fRegisteredItems;
    public:
        virtual ~EExtensionRegister() = default;
        /**
         * Register a new Item from the plugin
         * @param extensionName The name of the extension.
         * @param item The item to be registered
         */
        void RegisterItem(const EString& extensionName, const T& item)
        {
            fRegisteredItems[extensionName].push_back(item);
        }

        /**
         * Clear the Items for a given Extension
         * @param extensionName The name of the Extension. 
         */
        void ClearRegisteredItems(const EString& extensionName)
        {
            fRegisteredItems[extensionName].clear();
        }
    };

}