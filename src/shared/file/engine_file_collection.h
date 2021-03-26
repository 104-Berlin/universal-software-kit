//-----------------------------------------------------------------------------
//----- Copyright 104-Berlin 2021 https://github.com/104-Berlin
//-----------------------------------------------------------------------------
#pragma once

namespace Engine {

    

    // A class that can pack multiple files into one
    // Like a zip file but with no compression
    class E_API EFileCollection
    {
        using FileMap = EUnorderedMap<EString, ESharedBuffer>;
    private:
        FileMap             fFileMap;
    public:
        // Initialisation
        EFileCollection() = default;
        EFileCollection(const EFileCollection& other) = default;
        ~EFileCollection() = default;

        // Iteration
        FileMap::iterator begin();
        FileMap::const_iterator begin() const;
        FileMap::iterator end();
        FileMap::const_iterator end() const;

        /**
         * 
         * @param path - path to file inside collection
         * @param oBuffer - pointer to resulting file buffer
         * 
         * @return wether the was found
        */
        bool GetFileAt(const EString& path, ESharedBuffer* oBuffer);
        /**
         * @param path - path to file inside collection
         * @param buffer - filebuffer to put inside colleciton
        */
        void AddFile(const EString& path, ESharedBuffer buffer);

        /**
         * @return a buffer packed with all the files
         */
        ESharedBuffer GetCompleteBuffer() const;
        /**
         * @param buffer - Buffer to create the FileCollection.
         *                 This buffer must been created with GetCompleteBuffer methode to get parsed correct
         */
        void SetFromCompleteBuffer(ESharedBuffer buffer);

    private:
        void WriteU32(u8* ptr, u32 value) const;
        void WriteU64(u8* ptr, u64 value) const;
    };

}