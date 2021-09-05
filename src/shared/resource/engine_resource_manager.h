#pragma once

namespace Engine {

    struct EResourceDescription
    {   
        using ImExFunction = std::function<byte*(const byte*)>;


        EVector<EString>    AccpetedFileEndings;
        ImExFunction        ImportFunction;
        ImExFunction        ExportFunction;
    };

    struct EResourceData
    {
        EString Type;
        EString Path;
        byte*   Data;
        size_t  DataSize;

        EResourceData()
            : Type(), Path(), Data(nullptr), DataSize(0)
        {

        }

        EResourceData(EString type, EString path, byte* data, size_t dataSize)
        {
            Type = type;
            Path = path;
            Data = data;
            DataSize = dataSize;
        }

        EResourceData(const EResourceData&) = default;

        template <typename T>
        void Convert(T& inValue)
        {
            T::ResourceConvert(Data, DataSize, inValue);
        }
    };

    class E_API EResourceManager
    {
    private:
        EUnorderedMap<EString, EResourceData> fLoadedResources;
    public:
        EResourceManager();
        ~EResourceManager();
        
        bool HasResource(const EString& path) const;
        void RegisterResource(const EString& type, const EString& path, byte* resourceData, size_t resourceDataSize);

        EResourceData GetResource(const EString& path) const;
        EVector<EResourceData> GetAllResource() const;
        EVector<EResourceData> GetAllResource(const EString& type) const;
    };

}