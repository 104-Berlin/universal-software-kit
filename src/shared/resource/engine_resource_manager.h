#pragma once

namespace Engine {


    struct EResourceDescription
    {   
        struct ResBuffer
        {
            byte*   Data = nullptr;
            size_t  Size = 0;
            
            byte*   UserData = nullptr;
            size_t  UserDataSize = 0;
        };
        struct RawBuffer
        {
            byte* Data = nullptr;
            size_t Size = 0;
        };
        using ImpFunction = std::function<ResBuffer(const RawBuffer)>;
        using ExpFunction = std::function<RawBuffer(const ResBuffer)>;

        EString             ResourceName;
        EVector<EString>    AcceptedFileEndings;
        ImpFunction         ImportFunction;
        ExpFunction         ExportFunction;

        EResourceDescription() = default;
        EResourceDescription(const EString& name, const EVector<EString>& acceptedFileEndings)
            : ResourceName(name), AcceptedFileEndings(acceptedFileEndings)
        {}
    };

    class FindResourceByType
    {
    private:
        EString fType;
    public:
        FindResourceByType(const EString& type)
            : fType(type)
        {}

        bool operator()(EResourceDescription other) const
        {
            return std::find(other.AcceptedFileEndings.begin(), other.AcceptedFileEndings.end(), fType) != other.AcceptedFileEndings.end();
        }
    };

    struct EResourceData
    {
        using t_ID = u64;

        t_ID    ID;
        EString Type;
        EString Name;
        EString PathToFile;
        byte*   Data;
        size_t  DataSize;
        byte*   UserData;
    private:
        size_t  UserDataSize;
    public:

        EResourceData(t_ID id = 0)
            : ID(id), Type(), Name(), PathToFile(), Data(nullptr), DataSize(0), UserData(nullptr), UserDataSize(0)
        {
            E_ASSERT_M(id, "Invalid id for resource data. Id cant be 0!");
        }

        EResourceData(t_ID id, const EString& type, const EString& name, byte* data, size_t dataSize)
            : ID(id), Type(type), Name(name), PathToFile(), UserData(nullptr), UserDataSize(0)
        {
            E_ASSERT_M(id, "Invalid id for resource data. Id cant be 0!");
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
            UserData = new byte[sizeof(T)];
            UserDataSize = sizeof(T);
        }

        void SetUserData(byte* data, size_t data_size)
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

        const byte* GetUserData() const
        {
            return UserData;
        }
    };

    class E_API EResourceManager
    {
    private:
        EUnorderedMap<EResourceData::t_ID, EResourceData*> fLoadedResources;
    public:
        EResourceManager();
        ~EResourceManager();
        
        bool HasResource(const EResourceData::t_ID& id) const;
        bool ImportResource(const EString& name, const EResourceDescription& description, byte* rawData, size_t data_size);
        bool ImportResourceFromFile(const EString& filePath, const EResourceDescription& description);
        bool ImportResourceFromFile(EFile& file, const EResourceDescription& description);
        

        EResourceData* GetResource(const EResourceData::t_ID& id) const;
        EVector<EResourceData*> GetAllResource() const;
        EVector<EResourceData*> GetAllResource(const EString& type) const;

        EResourceData::t_ID CreateNewId();
    };

}