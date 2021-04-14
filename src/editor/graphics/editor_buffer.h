#pragma once


namespace Editor {


	enum class EShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Matrix3, Matrix4, Int, Int2, Int3, Int4, UInt, Byte4, Bool, Texture2D, TextureCube
	};

	static u32 ShaderDataTypeSize(EShaderDataType type)
	{
		switch (type)
		{
		case EShaderDataType::Float:			return 4;
		case EShaderDataType::Float2:			return 4 * 2;
		case EShaderDataType::Float3:			return 4 * 3;
		case EShaderDataType::Float4:			return 4 * 4;
		case EShaderDataType::Matrix3:			return 4 * 3 * 3;
		case EShaderDataType::Matrix4:			return 4 * 4 * 4;
		case EShaderDataType::Int:				return 4;
		case EShaderDataType::Texture2D:		return 4;
		case EShaderDataType::TextureCube:		return 4;
		case EShaderDataType::Int2:				return 4 * 2;
		case EShaderDataType::Int3:				return 4 * 3;
		case EShaderDataType::Int4:				return 4 * 4;
		case EShaderDataType::UInt:				return 4;
		case EShaderDataType::Byte4:			return 4;
		case EShaderDataType::Bool:				return 1;
		case EShaderDataType::None:				break;
		}

		std::cout << "Unknown EShaderDataType" << std::endl;
		return 0;
	}

	static EShaderDataType ShaderDataTypeFromString(const EString& typeString)
	{
		if (typeString == "float") return EShaderDataType::Float;
		else if (typeString == "vec2") return EShaderDataType::Float2;
		else if (typeString == "vec3") return EShaderDataType::Float3;
		else if (typeString == "vec4") return EShaderDataType::Float4;
		else if (typeString == "bool") return EShaderDataType::Bool;
		else if (typeString == "dont know") return EShaderDataType::Byte4;
		else if (typeString == "int") return EShaderDataType::Int;
		else if (typeString == "ivec2") return EShaderDataType::Int2;
		else if (typeString == "ivec3") return EShaderDataType::Int3;
		else if (typeString == "ivec4") return EShaderDataType::Int4;
		else if (typeString == "mat3") return EShaderDataType::Matrix3;
		else if (typeString == "mat4") return EShaderDataType::Matrix4;
		else if (typeString == "sampler2D") return EShaderDataType::Texture2D;
		else if (typeString == "samplerCube") return EShaderDataType::TextureCube;

		return EShaderDataType::None;
	}

	struct EBufferElement
	{
		EShaderDataType Type;
		EString Name;
		size_t Size;
		size_t Offset;
		bool Normalized;

		EBufferElement() {}

		EBufferElement(EShaderDataType type, const EString& name, bool normalized = false)
			: Name(name), Type(type), Normalized(normalized), Size(ShaderDataTypeSize(type)), Offset(0)
		{}

		u32 GetComponentCount() const
		{
			switch (Type)
			{
			case EShaderDataType::Float:		return 1;
			case EShaderDataType::Float2:		return 2;
			case EShaderDataType::Float3:		return 3;
			case EShaderDataType::Float4:		return 4;
			case EShaderDataType::Matrix3:		return 3 * 3;
			case EShaderDataType::Matrix4:		return 4 * 4;
			case EShaderDataType::Int:			return 1;
			case EShaderDataType::Int2:			return 2;
			case EShaderDataType::Int3:			return 3;
			case EShaderDataType::Int4:			return 4;
			case EShaderDataType::UInt:			return 1;
			case EShaderDataType::Byte4:		return 4;
			case EShaderDataType::Bool:			return 1;
			case EShaderDataType::Texture2D: 	return 1;
			case EShaderDataType::TextureCube:	return 1;
			case EShaderDataType::None:			break;
			}

			//TODO: Add ASSERT
			//IN_CORE_ASSERT(false, "Unknown Shader Type");
			return 0;
		}
	};

	class EBufferLayout
	{
	public:
		EBufferLayout() {}

		EBufferLayout(const std::initializer_list<EBufferElement>& elements)
			: fElements(elements)
		{
			CalculateOffsetAndStride();
		}

		inline u32 GetStride() const { return fStride; }
		inline const std::vector<EBufferElement>& GetElements() const { return fElements; }

		std::vector<EBufferElement>::iterator begin() { return fElements.begin(); }
		std::vector<EBufferElement>::iterator end() { return fElements.end(); }
		std::vector<EBufferElement>::const_iterator begin() const { return fElements.begin(); }
		std::vector<EBufferElement>::const_iterator end() const { return fElements.end(); }
	private:
		void CalculateOffsetAndStride()
		{
			u32 offset = 0;
			fStride = 0;
			for (auto& element : fElements)
			{
				element.Offset = offset;
				offset += element.Size;
				fStride += element.Size;
			}
		}
	private:
		std::vector<EBufferElement> fElements;
		u32 fStride = 0;
	};

	enum class EBufferUsage
	{
		STREAM_DRAW, 
		STREAM_READ, 
		STREAM_COPY, 
		STATIC_DRAW, 
		STATIC_READ, 
		STATIC_COPY, 
		DYNAMIC_DRAW, 
		DYNAMIC_READ, 
		DYNAMIC_COPY
	};

	class EVertexBuffer
	{
	protected:
		EBufferUsage fBufferUsage;
	public:
		EVertexBuffer(EBufferUsage usage = EBufferUsage::STATIC_DRAW);
		virtual ~EVertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const EBufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const EBufferLayout& layout) = 0;

		// Call this in a IN_RENDER({})
		virtual void* Map() = 0;
		// DO NOT Call this in a IN_RENDER({})
		virtual void Unmap() = 0;

		virtual void SetData(u8* data, u32 size) = 0;

		static ERef<EVertexBuffer> Create(EBufferUsage usage = EBufferUsage::STATIC_DRAW);
		static ERef<EVertexBuffer> Create(u8* data, u32 size);
	};

	class EIndexBuffer
	{
	protected:
		EBufferUsage fBufferUsage;
	public:
		EIndexBuffer(EBufferUsage usage = EBufferUsage::STATIC_DRAW);
		virtual ~EIndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual u32 GetCount() const = 0;

		virtual void SetData(u8* data, u32 size_in_bytes, u32 element_size) = 0;

		static ERef<EIndexBuffer> Create(EBufferUsage usage = EBufferUsage::STATIC_DRAW);
		static ERef<EIndexBuffer> Create(u8* data, u32 size_in_bytes, u32 element_size);
	};

	class EVertexArray
	{
	public:
		virtual ~EVertexArray() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		
		virtual void AddVertexBuffer(const ERef<EVertexBuffer>& vertexBuffer) = 0;
		virtual const ERef<EIndexBuffer>& GetIndexBuffer() const = 0;
		virtual void SetIndexBuffer(const ERef<EIndexBuffer>& indexBuffer) = 0;

		static ERef<EVertexArray> Create();
	};





	
	enum class EFramebufferFormat
	{
		None = 0,
		RGBA8 = 1,
		RGBA16F = 2
	};

	class EFrameBuffer
	{
	public:
		virtual ~EFrameBuffer() {}

		virtual void Resize(u32 width, u32 height) = 0;

		virtual u32 GetWidth() const = 0;
		virtual u32 GetHeight() const = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void BindTexture(u32 slot = 0) const = 0;

		virtual u32 GetColorAttachment() const { return 0; }

		static ERef<EFrameBuffer> Create(u32 width, u32 height, EFramebufferFormat format);
	};




}