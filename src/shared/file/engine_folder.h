#pragma once

namespace Engine {

    enum EBaseFolder
    {
        RES,
        APPLICATION,
        PLUGIN,
    };



    constexpr i32 EFolderOptions_None = 0;
    constexpr i32 EFolderOptions_CreateIfNotExist = BIT(0);

    /**
     * Representation of a Folder inside the Engine
     */
    class E_API EFolder
    {
    public:
    private:
        EString         fFullPath;
        i32             fFolderOptions;
    public:
        /**
         * @param path - path to folder
         * @param folderOptions - additional bitfield options for the folder
         */
        EFolder(const EString& path, i32 folderOptions = EFolderOptions_None);
        /**
         * @param f - BaseFolder path 
         * @param path - Path to append to BaseFolder
         * @param folderOptions - additional bitfield options for the folder
         */
        EFolder(EBaseFolder f, const EString& path = "", i32 folderOptions = EFolderOptions_None);

        /**
         * @return lexically correct full path to folder
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