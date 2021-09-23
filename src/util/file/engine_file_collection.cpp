#include "engine.h"
#include "prefix_util.h"

using namespace Engine;

EFileCollection::FileMap::iterator EFileCollection::begin() 
{
    return fFileMap.begin();
}

EFileCollection::FileMap::const_iterator EFileCollection::begin() const
{
    return fFileMap.begin();
}

EFileCollection::FileMap::iterator EFileCollection::end() 
{
    return fFileMap.end();
}

EFileCollection::FileMap::const_iterator EFileCollection::end() const
{
    return fFileMap.end();
}

bool EFileCollection::GetFileAt(const EString& path, ESharedBuffer* oBuffer) 
{
    if (!oBuffer) { return false; }
    FileMap::iterator it = fFileMap.find(path);
    if (it != fFileMap.end())
    {
        *oBuffer = it->second;
        return true;
    }    
    return false;
}

void EFileCollection::AddFile(const EString& path, ESharedBuffer buffer) 
{
    if (path.empty()) { return; }
    if (fFileMap.find(path) == fFileMap.end())
    {
        fFileMap[path] = buffer;
    }
}

ESharedBuffer EFileCollection::GetCompleteBuffer() const
{
    size_t buffer_size = 0;
    size_t first_file_pos = 0;
    // FileCount - u32
    // null-terminated string - u8[]
    // FileStart - u64
    // FileSize - u64
    // .... * FileCount
    // All Files behind

    buffer_size += sizeof(u32); // FileCount
    first_file_pos += sizeof(u32); // FileCount
    for (const auto& entry : fFileMap)
    {
        size_t string_size = entry.first.length() + 1;
        buffer_size += string_size + (2 * sizeof(u64));
        first_file_pos += string_size + (2 * sizeof(u64));

        buffer_size += entry.second.GetSizeInByte();
    }

    ESharedBuffer result = ESharedBuffer();
    result.InitWith<u8>(buffer_size);
    u8* buffer = result.Data<u8>();
    u8* ptr = buffer;

    // Set FileCount
    WriteU32(ptr, (u32) fFileMap.size());
    ptr += sizeof(u32);

    // Fill the strings
    size_t current_file_offset = 0;
    for (auto& entry : fFileMap)
    {
        strcpy((char*) ptr, entry.first.c_str());
        ptr += entry.first.size() + 1;

        // File Start
        u64 file_start = first_file_pos + current_file_offset;
        WriteU64(ptr, file_start);
        ptr += sizeof(u64);

        
        // FileSize
        u64 fileSize = entry.second.GetSizeInByte();
        WriteU64(ptr, fileSize);
        ptr += sizeof(u64);

        // Increase file offset
        current_file_offset += entry.second.GetSizeInByte();

        // Write the file
        u8* file_ptr = buffer + file_start;
        ESharedBuffer data = entry.second;
        memcpy((void*) file_ptr, data.Data<void>(), entry.second.GetSizeInByte());
    }

    return result;
}

void EFileCollection::SetFromCompleteBuffer(ESharedBuffer buffer) 
{
    u8* file_ptr = buffer.Data<u8>();
    u8* ptr = file_ptr;

    // FileCount
    u32 fileCount = *(u32*)ptr;
    ptr += sizeof(u32);

    for (size_t i = 0; i < fileCount; i++)
    {
        // File Path
        EString filePath = EString((char*)ptr);
        ptr += filePath.length() + 1;
        
        // File Start pos
        u64 fileStartPos = *(u64*)ptr;
        ptr += sizeof(u64);

        // File size
        u64 fileSize = *(u64*)ptr;
        ptr += sizeof(u64);

        u8* file_buffer_ptr = file_ptr + fileStartPos;

        ESharedBuffer newBuffer;
        newBuffer.InitWith<u8>(fileSize);
        memcpy(newBuffer.Data(), file_ptr + fileStartPos, fileSize);
        fFileMap[filePath] = newBuffer;
    }
}

void EFileCollection::WriteU32(u8* ptr, u32 value)
{
    ptr[0] = (u8)( ( value ) & 0xFF );
    ptr[1] = (u8)( ( value ) >> 8 );
    ptr[2] = (u8)( ( value >> 16 ) & 0xFF );
    ptr[3] = (u8)( ( value >> 16 ) >> 8 );
}

void EFileCollection::WriteU64(u8* ptr, u64 value) 
{
    ptr[0] = (u8)( ( value ) & 0xFF );
    ptr[1] = (u8)( ( value ) >> 8 );
    ptr[2] = (u8)( ( value >> 16 ) & 0xFF );
    ptr[3] = (u8)( ( value >> 16 ) >> 8 );
    ptr[4] = (u8)( ( value >> 32 ) & 0xFF );
    ptr[5] = (u8)( ( value >> 32 ) >> 8 );
    ptr[6] = (u8)( ( value >> 48 ) & 0xFF );
    ptr[7] = (u8)( ( value >> 48 ) >> 8 );
}
