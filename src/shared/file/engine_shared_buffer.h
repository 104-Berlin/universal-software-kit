#pragma once


class E_API ESharedBuffer
{
private:
    class BufferDataBase
    {
    public:
        virtual ~BufferDataBase() = default;

        virtual void Create(void* data, size_t size_in_bytes) = 0;
        virtual void Delete() = 0;
        virtual void* Get() = 0;
    };

    template <typename T>
    class InternBuffer : public BufferDataBase
    {
    private:
        T* fData;
    public:
        InternBuffer()
            : fData(nullptr)
        {
            
        }

        virtual void Create(void* data, size_t size_in_bytes)
        {
            fData = new T[size_in_bytes /  sizeof(T)];
            if (data)
            {
                memcpy(fData, data, size_in_bytes);
            }
            else
            {
                memset(fData, 0, size_in_bytes);
            }
        }

        virtual void Delete()
        {
            delete [] (T*)fData;
        }

        virtual void* Get()
        {
            return (void*) fData;
        }
    };

    BufferDataBase* fBuffer;
    int*            fRefCount;
    size_t          fSizeInBytes;
    u32             fElementCount;
public:
    ESharedBuffer();
    ESharedBuffer(const ESharedBuffer& other);
    void operator=(const ESharedBuffer& other);
    ~ESharedBuffer();


    template <typename PointerType>
    ESharedBuffer& InitWith(size_t size_in_bytes)
    {
        InitWith<PointerType>(nullptr, size_in_bytes);

        return *this;
    }

    /*
        @param data - Used to copy into this buffer. 
                        When set nullptr buffer will be created with given size and set all memory to zero.
        @param size_in_bytes - the size of the buffer in bytes
    */
    template <typename PointerType>
    ESharedBuffer& InitWith(void* data, size_t size_in_bytes)
    {
        fRefCount = new int(1);
        fBuffer = new InternBuffer<PointerType>();
        fBuffer->Create(data, size_in_bytes);
        fSizeInBytes = size_in_bytes;
        fElementCount = size_in_bytes / sizeof(PointerType);

        return *this;
    }

    void* Data()
    {
        return fBuffer->Get();
    }

    template <typename T>
    T* Data()
    {
        return (T*)fBuffer->Get();
    }

    u32 GetElementCount() const;
    size_t GetSizeInByte() const;

    bool IsNull() const;
    void Dispose();
};

