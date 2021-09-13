#pragma once

namespace Engine {

    class ESharedBuffer
    {
    private:
        class BufferDataBase
        {
        public:
            virtual ~BufferDataBase() = default;

            virtual void Create(const void* data, size_t size_in_bytes) = 0;
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

            virtual void Create(const void* data, size_t size_in_bytes)
            {
                fData = new T[size_in_bytes / sizeof(T)];
                if (data)
                {
                    if (size_in_bytes / sizeof(T) == 1)
                    {
                        *fData = T(*(T*)data);
                    }
                    else
                    {
                        memcpy(fData, data, size_in_bytes);
                    }
                }
                else
                {
                    if (size_in_bytes / sizeof(T) == 1)
                    {
                        *fData = T();
                    }
                    else
                    {
                        memset(fData, 0, size_in_bytes);
                    }
                }
            }

            virtual void Delete()
            {
                delete [] fData;
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
        /**
         * Create empty buffer
         */
        ESharedBuffer();

        ESharedBuffer(const ESharedBuffer& other);
        void operator=(const ESharedBuffer& other);
        ~ESharedBuffer();

        /**
         * Init the buffer with empty data
         * @param size_in_bytes The size in bytes
         */
        template <typename PointerType>
        ESharedBuffer& InitWith(size_t size_in_bytes)
        {
            InitWith<PointerType>(nullptr, size_in_bytes);

            return *this;
        }

        /*
            @param data Used to copy into this buffer. 
                            When set nullptr buffer will be created with given size and set all memory to zero.
            @param size_in_bytes The size of the buffer in bytes
        */
        template <typename PointerType>
        ESharedBuffer& InitWith(const void* data, size_t size_in_bytes)
        {
            fRefCount = new int(1);
            fSizeInBytes = size_in_bytes;
            fElementCount = size_in_bytes / sizeof(PointerType);

            fBuffer = new InternBuffer<PointerType>();
            fBuffer->Create(data, size_in_bytes);

            return *this;
        }

        void* Data()
        {
            return fBuffer->Get();
        }

        const void* Data() const
        {
            return fBuffer->Get();
        }

        template <typename T>
        T* Data()
        {
            return (T*)fBuffer->Get();
        }

        template <typename T>
        const T* Data() const
        {
            return (T*)fBuffer->Get();
        }

        u32 GetElementCount() const;
        size_t GetSizeInByte() const;

        bool IsNull() const;
        /**
         * Removes reference to memory and deletes it if no one got an handle to it. The destructor will call the same function
         */
        void Dispose();
    };

    
}