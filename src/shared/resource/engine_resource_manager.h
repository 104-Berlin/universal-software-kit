#pragma once

namespace Engine {


    struct EResourceDescription
    {   
        struct ResBuffer
        {
            byte*   Data;
            size_t  Size;
        };
        using ImExFunction = std::function<ResBuffer(const ResBuffer)>;

        EString             ResourceName;
        EVector<EString>    AcceptedFileEndings;
        ImExFunction        ImportFunction;
        ImExFunction        ExportFunction;

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

        EString Type;
        EString Name;
        EString PathToFile;
        byte*   Data;
        size_t  DataSize;

        EResourceData()
            : Type(), Name(), PathToFile(), Data(nullptr), DataSize(0)
        {

        }

        EResourceData(const EString& type, const EString& name, byte* data, size_t dataSize)
            : Type(type), Name(name), PathToFile()
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
    };

    class E_API EResourceManager
    {
    private:
        EUnorderedMap<EResourceData::t_ID, EResourceData*> fLoadedResources;
    public:
        EResourceManager();
        ~EResourceManager();
        
        bool HasResource(const EResourceData::t_ID& id) const;
        void RegisterResource(const EString& type, const EString& path, byte* resourceData, size_t resourceDataSize);

        EResourceData* GetResource(const EResourceData::t_ID& id) const;
        EVector<EResourceData*> GetAllResource() const;
        EVector<EResourceData*> GetAllResource(const EString& type) const;

        EResourceData::t_ID CreateNewId();
    };

}