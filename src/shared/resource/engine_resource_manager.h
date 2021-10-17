#pragma once

namespace Engine {


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
        using ImpFunction = std::function<ResBuffer(const RawBuffer)>;
        using ExpFunction = std::function<RawBuffer(const ResBuffer)>;

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


    struct EResourceData
    {
        using t_ID = u64;

        t_ID    ID;
        EString Type;
        EString Name;
        EString PathToFile;
        u8*   Data;
        u64     DataSize;
        u8*   UserData;
        u64     UserDataSize;
    public:

        EResourceData(t_ID id = 0)
            : ID(id), Type(), Name(), PathToFile(), Data(nullptr), DataSize(0), UserData(nullptr), UserDataSize(0)
        {
            
        }

        EResourceData(t_ID id, const EString& type, const EString& name, u8* data, size_t dataSize)
            : ID(id), Type(type), Name(name), PathToFile(), UserData(nullptr), UserDataSize(0)
        {
            Data = data;
            DataSize = dataSize;
        }

        EResourceData(const EResourceData&) = default;

        ~EResourceData()
        {
            if (Data)
            {
                delete Data;
                Data = nullptr;
            }
        }

        template <typename T>
        void SetUserData(const T& data)
        {
            if (UserData)
            {
                delete[] UserData;
            }
            UserData = new u8[sizeof(T)];
            UserDataSize = sizeof(T);
        }

        void SetUserData(u8* data, size_t data_size)
        {
            E_ASSERT(data);
            if (UserData)
            {
                delete[] UserData;
            }
            UserData = data;
            UserDataSize = data_size;
        }

        template <typename T>
        const T* GetUserData() const
        {
            E_ASSERT(UserDataSize == sizeof(T));
            return static_cast<T*>(UserData);
        }

        const u8* GetUserData() const
        {
            return UserData;
        }
    };

    namespace events {

        E_STORAGE_STRUCT(EResourceAddedEvent,
            (EResourceData::t_ID, ResourceID),
            (EString, Name),
            (EString, PathToFile),
            (EString, ResourceType)
        )

    }

    class E_API EResourceManager
    {
    private:
        EUnorderedMap<EResourceData::t_ID, EResourceData*> fLoadedResources;
        EEventDispatcher                                   fEventDispacher;
    public:
        EResourceManager();
        ~EResourceManager();
        
        bool HasResource(const EResourceData::t_ID& id) const;
        bool AddResource(EResourceData* data); // Delete the Resource data if function returns false!!
        bool ImportResource(const EString& name, const EResourceDescription& description, u8* rawData, size_t data_size);
        bool ImportResourceFromFile(const EString& filePath, const EResourceDescription& description);
        bool ImportResourceFromFile(EFile& file, const EResourceDescription& description);

        void Clear();
        

        EResourceData* GetResource(const EResourceData::t_ID& id) const;
        EVector<EResourceData*> GetAllResource() const;
        EVector<EResourceData*> GetAllResource(const EString& type) const;

        EResourceData::t_ID CreateNewId();

        EEventDispatcher& GetEventDispatcher();
        const EEventDispatcher& GetEventDispatcher() const;

        static EResourceData* CreateResourceFromFile(EFile& file, const EResourceDescription& description);
        static EResourceData* CreateResourceFromFile(const EString& filePath, const EResourceDescription& description);

    };

}