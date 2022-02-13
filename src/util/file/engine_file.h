#pragma once

namespace Engine {

    namespace Path {

        /**
         * Joins the path with the system path seperator
         */
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
    /**
     * Representation of a File inside the Engine
     */
    class EFile
    {
    private:
        EString fFilePath;
        EString fFileExtension;
        EString fFileName;

        ESharedBuffer fFileBuffer;
    public:
        /**
         * @param path Relative or absolute path to file (dont need to exist)
         */
        EFile(const EString& path);
        /**
         * @param baseFolder Start from a predefined folder
         * @param path Relative path from the start of the base folder to file
         */
        EFile(EBaseFolder baseFolder, const EString& path);
        ~EFile();

        bool Exist() const;

        /**
         * @return Lexical correct full path to file
         */
        EString GetFullPath() const;
        /**
         * @return Path passed in creation
         */
        const EString& GetPath() const;
        /**
         * @return The file extension without the dot
         */
        const EString& GetFileExtension() const;
        /**
         * @return The raw name of the file without extension 
         */
        const EString& GetFileName() const;


        /**
         * @param extensions A list of all extensions to check for
         */
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

        /**
         * @return Reads the file as string and returns it
         */
        EString GetFileAsString() const;
        /**
         * Writes a string to the file
         * @param string The string to write
         */
        void SetFileAsString(const EString& string) const;

        /**
         * This function will store the file as buffer in the file
         */
        void LoadToMemory();
        /**
         * This function will clear the file buffer if its been filled
         */
        void DisposeMemory();
        
        /**
         * This fill save the current loaded buffer to disk
         */
        void SaveBufferToDisk();

        /**
         * @return The file as buffer. Buffer must be filled via LoadToMemory()
         */
        ESharedBuffer GetBuffer() const;
        /**
         * Override the Buffer. Use SaveBufferToDisk for saving the buffer to HardDrive
         * @param sharedBuffer The buffer to override to current loaded
         */
        void SetFileBuffer(ESharedBuffer sharedBuffer);


        /**
         * @return Returns the temporary file directory for the app
         */
        static EString GetTempPath();
    private:
        void CreatePathStrings();
    };

}
