#pragma once

namespace Editor {

	static GLenum ShaderDataTypeToOpenGLType(EShaderDataType type)
	{
		switch (type)
		{
		case EShaderDataType::Float:			
		case EShaderDataType::Float2:			
		case EShaderDataType::Float3:			
		case EShaderDataType::Float4:			
		case EShaderDataType::Matrix3:			
		case EShaderDataType::Matrix4:			return GL_FLOAT;
		case EShaderDataType::Int:				
		case EShaderDataType::Texture2D:		
		case EShaderDataType::TextureCube:		
		case EShaderDataType::Int2:				
		case EShaderDataType::Int3:				
		case EShaderDataType::Int4:				return GL_INT;
		case EShaderDataType::UInt:				return GL_UNSIGNED_INT;
		case EShaderDataType::Byte4:			return GL_UNSIGNED_BYTE;
		case EShaderDataType::Bool:				return GL_BOOL;
		case EShaderDataType::None:				break;
		}
		return 0;
	}

	class EOpenGLVertexBuffer : public EVertexBuffer
	{
	public:
		EOpenGLVertexBuffer(EBufferUsage usage = EBufferUsage::STREAM_DRAW);
		EOpenGLVertexBuffer(u8* data, u32 data_size, EBufferUsage usage = EBufferUsage::STATIC_DRAW);
		~EOpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void* Map() override;
		virtual void Unmap() override;

		virtual void SetData(u8* data, u32 data_size) override;

		virtual const EBufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const EBufferLayout& layout) override { m_Layout = layout; }
	private:
		void PrivBind() const;
		u32 m_RendererID = 0;
		EBufferLayout m_Layout;
	};

	class EOpenGLIndexBuffer : public EIndexBuffer
	{
	public:
		EOpenGLIndexBuffer(EBufferUsage usage = EBufferUsage::STREAM_DRAW);
		EOpenGLIndexBuffer(u8* data, u32 size_in_bytes, u32 element_size, EBufferUsage usage = EBufferUsage::STATIC_DRAW);
		~EOpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(u8* data, u32 size_in_bytes, u32 element_size) override;

		virtual u32 GetCount() const override { return m_Count; }
	private:
		u32 m_RendererID = 0;
		u32 m_Count;
	};

	class EOpenGLVertexArray : public EVertexArray
	{
	public:
		EOpenGLVertexArray();
		~EOpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const ERef<EVertexBuffer>& vertexBuffer) override;
		virtual const ERef<EIndexBuffer>& GetIndexBuffer() const override;
		virtual void SetIndexBuffer(const ERef<EIndexBuffer>& indexBuffer) override;
	private:
		u32 fVertexBufferIndex = 0;
		u32 fRendererID = 0;
		EVector<ERef<EVertexBuffer>> fVertexBuffers;
		ERef<EIndexBuffer> fIndexBuffer;
	};



	class EOpenGLFrameBuffer : public EFrameBuffer
	{
	public:
		EOpenGLFrameBuffer(u32 width, u32 height, EFramebufferFormat format);
		virtual ~EOpenGLFrameBuffer();

		void Resize(u32 width, u32 height) override;

		virtual u32 GetWidth() const override { return m_Width; }
		virtual u32 GetHeight() const override { return m_Height; }

		void Bind() const override;
		void Unbind() const override;

		void BindTexture(u32 slot = 0) const override;

		u32 GetColorAttachment() const override { return m_ColorAttachment; }
	private:
		u32 m_RendererID;
		u32 m_Width;
		u32 m_Height;
		EFramebufferFormat m_Format;
		u32 m_ColorAttachment;
		u32 m_DepthAttachment;
	};

}
