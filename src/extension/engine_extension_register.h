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

        /**
         * @return The total count of all alements by all extensions
         */
        u32 GetItemCount() const
        {
            u32 result = 0;
            for (const auto& entry : fRegisteredItems)
            {
                result += entry.second.size();
            }
            return result;
        }

        /**
         * Get all items
         * @return List of all item registered by all extensions
         */
        EVector<T> GetAllItems() const
        {
            EVector<T> result;

            size_t currentIndx = 0;
            for (const auto& entry : fRegisteredItems)
            {
                result.insert(result.begin() + currentIndx, entry.second.begin(), entry.second.end());
                currentIndx += entry.second.size();
            }
            return result;
        }
    };
    

}