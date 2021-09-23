#include "engine.h"
#include "prefix_util.h"

using namespace Engine;


ESharedBuffer::ESharedBuffer() 
{
    fBuffer = nullptr;
    fRefCount = nullptr;
    fSizeInBytes = 0;
    fElementCount = 0;
}

ESharedBuffer::ESharedBuffer(const ESharedBuffer& other) 
{
    fBuffer = other.fBuffer;
    fRefCount = other.fRefCount;
    fSizeInBytes = other.fSizeInBytes;
    fElementCount = other.fElementCount;
    if (!IsNull())
    {
        (*fRefCount)++;
    }
}

void ESharedBuffer::operator=(const ESharedBuffer& other) 
{
    fBuffer = other.fBuffer;
    fRefCount = other.fRefCount;
    fSizeInBytes = other.fSizeInBytes;
    fElementCount = other.fElementCount;
    if (!IsNull())
    {
        (*fRefCount)++;
    }
}


ESharedBuffer::~ESharedBuffer() 
{
    Dispose();
}

u32 ESharedBuffer::GetElementCount() const
{
    return fElementCount;
}

size_t ESharedBuffer::GetSizeInByte() const
{
    return fSizeInBytes;
}

bool ESharedBuffer::IsNull() const
{
    return !fBuffer || !fRefCount;
}

void ESharedBuffer::Dispose() 
{
    if (!IsNull())
    {
        (*fRefCount)--;
        if ((*fRefCount) == 0)
        {
            fBuffer->Delete();

            delete fRefCount;
            delete fBuffer;

            fBuffer = nullptr;
            fRefCount = nullptr;
        }
    }
}

