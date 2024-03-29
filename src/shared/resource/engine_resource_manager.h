#pragma once

namespace Engine {

    class E_API EResource 
    {
    public:
        using t_ID = u64;
    private:
        t_ID fID;
        ESharedBuffer fFileBuffer;

        EString         fType;
        EString         fName;

        void*           fCPtr;
        size_t          fPtrTypeHash;
    public:
        EResource(const EString& type);

        void SetID(const t_ID& id);
        const t_ID& GetID() const;
        void SetBuffer(ESharedBuffer buffer);
        const ESharedBuffer& GetBuffer() const;
        void SetName(const EString& name);
        const EString& GetName() const;

        const EString& GetResourceType() const;

        EString GetTempFilePath() const;


        template <typename T>
        auto Load()
        -> decltype(std::declval<T>().FromBuffer(std::declval<ESharedBuffer>()))
        {
            if (fFileBuffer.IsNull())
            {
                E_ERROR("EResource::Load: No buffer set");
                return;
            }
            fPtrTypeHash = typeid(T).hash_code();
            fCPtr = new T();
            ((T*)fCPtr)->FromBuffer(fFileBuffer);
        }

        template <typename T>
        T* GetCPtr()
        {
            if (!fCPtr)
            {
                Load<T>();
            }
            if (typeid(T).hash_code() != fPtrTypeHash)
            {
                E_ERROR("EResource::GetCPtr: Wrong type");
                return nullptr;
            }
            return (T*)fCPtr;
        }

        
    };


    struct EResourceDescription
    {   
        struct ResBuffer
        {
            u8*   Data = nullptr;
            size_t  Size = 0;
            
            u8*   UserData = nullptr;
            size_t  UserDataSize = 0;
        };
        struct RawBuffer
        {
            u8* Data = nullptr;
            size_t Size = 0;
        };
        using ImpFunction = std::function<EResource*(ESharedBuffer)>;
        using ExpFunction = std::function<ESharedBuffer(EResource*)>;

        EString             ResourceName;
        EVector<EString>    AcceptedFileEndings;
        ImpFunction         ImportFunction;
        ExpFunction         ExportFunction;
        bool                CanCreate;

        EResourceDescription() = default;
        EResourceDescription(const EString& name, const EVector<EString>& acceptedFileEndings)
            : ResourceName(name), AcceptedFileEndings(acceptedFileEndings), CanCreate(false)
        {}
    };

    namespace events {

        E_STORAGE_STRUCT(EResourceAddedEvent,
            (EResource::t_ID, ResourceID),
            (EString, Name),
            (EString, PathToFile),
            (EString, ResourceType)
        )

    }

    class E_API EResourceManager
    {
    private:
        EUnorderedMap<EResource::t_ID, EResource*> fLoadedResources;
        EEventDispatcher                                   fEventDispacher;
    public:
        EResourceManager();
        ~EResourceManager();
        
        bool HasResource(const EResource::t_ID& id) const;
        bool AddResource(EResource* data); // Delete the Resource data if function returns false!!
        bool ImportResource(const EString& name, const EResourceDescription& description, u8* rawData, size_t data_size);
        bool ImportResourceFromFile(const EString& filePath, const EResourceDescription& description);
        bool ImportResourceFromFile(EFile& file, const EResourceDescription& description);

        void Clear();
        

        EResource* GetResource(const EResource::t_ID& id) const;
        EVector<EResource*> GetAllResource() const;
        EVector<EResource*> GetAllResource(const EString& type) const;

        EResource::t_ID CreateNewId();

        EEventDispatcher& GetEventDispatcher();
        const EEventDispatcher& GetEventDispatcher() const;

        static EResource* CreateResourceFromFile(EFile& file, const EResourceDescription& description);
        static EResource* CreateResourceFromFile(const EString& filePath, const EResourceDescription& description);

    };

}