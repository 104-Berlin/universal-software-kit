#pragma once

namespace Engine {

    namespace Path {
        template <typename ...T>
        EString Join(const T&... args)
        {
            EString result;
            std::vector<EString> strings = {args...};
            for (size_t i = 0; i < strings.size() - 1; i++)
            {
                result += strings[i] + kPathSeparator;
            }
            result += strings[strings.size() - 1];
            return result;
        }
    }

    class E_API EFile
    {
    private:
        EString fFilePath;
        EString fFileExtension;
        EString fFileName;

        ESharedBuffer fFileBuffer;
    public:
        EFile(const EString& path);
        EFile(EBaseFolder baseFolder, const EString& path);
        ~EFile();

        bool Exist() const;

        EString GetFullPath() const;
        const EString& GetPath() const;
        const EString& GetFileExtension() const;
        const EString& GetFileName() const;


        template <typename ...T>
        bool HasExtension(const T&... extensions) const
        {
            const EString& thisExtension = GetFileExtension();
            std::vector<const char*> strings = {extensions...};
            for (const char* extension : strings)
            {
                if (thisExtension.compare(extension) == 0)
                {
                    return true;
                }
            }
            return false;
        }

        EString GetFileAsString() const;
        void SetFileAsString(const EString& string) const;

        // This function will store the file as buffer in the file
        void LoadToMemory();
        // This function will dispose the file buffer if its been filled
        void DisposeMemory();

        void SaveBufferToDisk();

        ESharedBuffer GetBuffer() const;
        void SetFileBuffer(ESharedBuffer sharedBuffer);
    private:
        void CreatePathStrings();
    };

}
