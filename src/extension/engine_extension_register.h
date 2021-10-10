#pragma once

namespace Engine {

    E_STORAGE_STRUCT(ERegisterChangedEvent,
        (EString, ExtensionName)
    )

    template <typename T>
    class EExtensionRegister
    {
    protected:
        EUnorderedMap<EString, EVector<T>> fRegisteredItems;
        EEventDispatcher                   fEventDispatcher;
    public:
        virtual ~EExtensionRegister()
        {
            if constexpr (std::is_pointer_v<T>){
                for (auto& entry : fRegisteredItems)
                {
                    for(T pointer : entry.second)
                    {
                        delete pointer;
                    }
                }
            }
        }
        /**
         * Register a new Item from the plugin
         * @param extensionName The name of the extension.
         * @param item The item to be registered
         */
        void RegisterItem(const EString& extensionName, const T& item)
        {
            fRegisteredItems[extensionName].push_back(item);
            ERegisterChangedEvent event;
            event.ExtensionName = extensionName;
            fEventDispatcher.Post<ERegisterChangedEvent>(event);
        }

        /**
         * Clear the Items for a given Extension
         * @param extensionName The name of the Extension. 
         */
        void ClearRegisteredItems(const EString& extensionName)
        {
            fRegisteredItems[extensionName].clear();
            ERegisterChangedEvent event;
            event.ExtensionName = extensionName;
            fEventDispatcher.Post<ERegisterChangedEvent>(event);
        }

        /**
         * Clears all the Items
         */
        void ClearAllItems()
        {
            fRegisteredItems.clear();
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

        /**
         * Gets the items from a special extension. use "CORE" for the default types
         * @return List of items registered by the extension
         */
        const EVector<T>& GetItems(const EString& extensionName)
        {
            return fRegisteredItems[extensionName];
        }

        /**
         * Finds an item with a callback function
         * @param testFunction Callback to test the element for correctness
         * @param outValue Pointer to put the found element
         * @return Wether the element was found
         */
        bool FindItem(std::function<bool(const T&)> testFunction, T* outValue)
        {
            E_ASSERT(outValue);
            
            for (const T& element : GetAllItems())
            {
                if (testFunction(element))
                {
                    *outValue = element;
                    return true;
                }
            }
            return false;
        }

        /**
         * Finds an item with a callback function
         * @param testFunction Callback to test the element for correctness
         * @param extensionName If you want to search a specific extension
         * @param outValue Pointer to put the found element
         * @return Wether the element was found
         */
        bool FindItem(std::function<bool(const T&)> testFunction, const EString& extensionName, T* outValue)
        {
            E_ASSERT(outValue);

            for (const T& element : GetItems(extensionName))
            {
                if (testFunction(element))
                {
                    *outValue = element;
                    return true;
                }
            }
            return false;
        }

        template <typename EventType, typename CB>
        void AddEventListener(CB&& callback)
        {
            fEventDispatcher.Connect<EventType>(callback);
        }
    };


    struct EComponentRegisterEntry
    {
        EValueDescription Description;
        ERef<EStructProperty> DefaultValue;
    };

    class EComponentRegister : public EExtensionRegister<EComponentRegisterEntry>
    {
    public:
        /**
         * Register a new type for extension. Use this function to make the default initilization work.
         * @param extensionName The name of the extension.
         */
        template <typename T>
        void RegisterStruct(const EString& extensionName)
        {
            EValueDescription dsc = getdsc::GetDescription<T>();
            if (dsc.Valid() && dsc.GetType() == EValueType::STRUCT)
            {
                T initValue = T();
                ERef<EStructProperty> initProperty = ERef<EStructProperty>(static_cast<EStructProperty*>(EProperty::CreateFromDescription(dsc.GetId(), dsc)));
                if (convert::setter<T>(initProperty.get(), initValue))
                {
                    E_WARN("Could not register the correct default value for type " + dsc.GetId());
                }
                RegisterItem(extensionName, {dsc, initProperty});
            }
        }
    };




    using EResourceRegister = EExtensionRegister<EResourceDescription>;




    
    class EFindTypeDescByName
    {
    private:
        EString fName;
    public:
        EFindTypeDescByName(const EString& name)
            : fName(name)
        {}

        bool operator()(EComponentRegisterEntry other) const
        {
            return other.Description.GetId() == fName;
        }
    };

     class EFindResourceByType
    {
    private:
        EString fType;
    public:
        EFindResourceByType(const EString& type)
            : fType(type)
        {}

        bool operator()(EResourceDescription other) const
        {
            return std::find(other.AcceptedFileEndings.begin(), other.AcceptedFileEndings.end(), fType) != other.AcceptedFileEndings.end();
        }
    };
}