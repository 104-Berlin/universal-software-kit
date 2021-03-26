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

    class E_API EFolder
    {
    public:
    private:
        EString         fFullPath;
        i32             fFolderOptions;
    public:
        EFolder(const EString& path, i32 folderOptions = EFolderOptions_None);
        EFolder(EBaseFolder f, const EString& path = "", i32 folderOptions = EFolderOptions_None);

        const EString& GetFullPath() const;

        bool Exist() const;
        void Create();

        std::filesystem::directory_iterator Iterator() const;

    public:
       static EString GetBaseFolderPath(EBaseFolder f);
    };

}