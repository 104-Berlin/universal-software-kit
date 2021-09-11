#pragma once

namespace Engine {

    enum EBaseFolder
    {
        RES,
        APPLICATION,
        PLUGIN,
    };



#define EFolderOptions_None 0
#define EFolderOptions_CreateIfNotExist BIT(0)

    /**
     * Representation of a Folder inside the Engine
     */
    class EFolder
    {
    public:
    private:
        EString         fFullPath;
        i32             fFolderOptions;
    public:
        /**
         * @param path Path to folder
         * @param folderOptions Additional bitfield options for the folder
         */
        EFolder(const EString& path, i32 folderOptions = EFolderOptions_None);
        /**
         * @param f BaseFolder path 
         * @param path Path to append to BaseFolder
         * @param folderOptions Additional bitfield options for the folder
         */
        EFolder(EBaseFolder f, const EString& path = "", i32 folderOptions = EFolderOptions_None);

        /**
         * @return Lexically correct full path to folder
         */
        const EString& GetFullPath() const;

        bool Exist() const;

        /**
         * Creates the folder on Disk
         */
        void Create();

        /**
         * Loop all files in folder
         */
        std::filesystem::directory_iterator Iterator() const;
    public:
       static EString GetBaseFolderPath(EBaseFolder f);
    };

}