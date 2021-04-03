#include "engine.h"
#include "prefix_shared.h"
#include "prefix_editor.h"

namespace Editor {

	static GLenum GetGLBufferUsage(EBufferUsage usage)
	{
		switch (usage)
		{
		case EBufferUsage::STREAM_DRAW: return GL_STREAM_DRAW;
		case EBufferUsage::STREAM_READ: return GL_STREAM_READ;
		case EBufferUsage::STREAM_COPY: return GL_STREAM_COPY;
		case EBufferUsage::STATIC_DRAW: return GL_STATIC_DRAW;
		case EBufferUsage::STATIC_READ: return GL_STATIC_READ;
		case EBufferUsage::STATIC_COPY: return GL_STATIC_COPY;
		case EBufferUsage::DYNAMIC_DRAW: return GL_DYNAMIC_DRAW;
		case EBufferUsage::DYNAMIC_READ: return GL_DYNAMIC_READ;
		case EBufferUsage::DYNAMIC_COPY: return GL_DYNAMIC_COPY;
		}
		return GL_NONE;
	}



	EOpenGLVertexBuffer::EOpenGLVertexBuffer(EBufferUsage usage) 
		: EVertexBuffer(usage)
	{
		IN_RENDER_S({
			glCall(glGenBuffers(1, &self->m_RendererID));
		})
	}

	
	EOpenGLVertexBuffer::EOpenGLVertexBuffer(u8* data, u32 data_size, EBufferUsage usage)
		: EVertexBuffer(usage)
	{
		Engine::ESharedBuffer sharedBuffer;
		sharedBuffer.InitWith<u8>(data, data_size);
		
		IN_RENDER_S2(sharedBuffer, usage, {
				glCall(glGenBuffers(1, &self->m_RendererID));
				glCall(glBindBuffer(GL_ARRAY_BUFFER, self->m_RendererID));
				glCall(glBufferData(GL_ARRAY_BUFFER, sharedBuffer.GetSizeInByte(), sharedBuffer.Data<void>(), GetGLBufferUsage(usage)));
			})
	}

	EOpenGLVertexBuffer::~EOpenGLVertexBuffer()
	{
		IN_RENDER_S({
				glCall(glDeleteBuffers(1, &self->m_RendererID));
			})
	}

	void EOpenGLVertexBuffer::Bind() const
	{
		IN_RENDER_S({
				glCall(self->PrivBind());
			})
	}

	void* EOpenGLVertexBuffer::Map()
	{
		return glCall(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	}

	void EOpenGLVertexBuffer::Unmap()
	{
		glCall(glUnmapBuffer(GL_ARRAY_BUFFER));
	}
	
	void EOpenGLVertexBuffer::SetData(u8* data, u32 data_size) 
	{
		Bind();
		Engine::ESharedBuffer sharedBuffer;
		sharedBuffer.InitWith<u8>(data, data_size);

		IN_RENDER_S1(sharedBuffer, {
			if (sharedBuffer)
			{
				glCall(glBufferData(GL_ARRAY_BUFFER, data.GetSizeInByte(), data.Data(), GetGLBufferUsage(self->fBufferUsage)));
			}
		})
	}
	

	void EOpenGLVertexBuffer::PrivBind() const 
	{
		glCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	}

	void EOpenGLVertexBuffer::Unbind() const
	{
#ifdef IN_DEBUG
		IN_RENDER({
				glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
			})
#endif
	}

	
	EOpenGLIndexBuffer::EOpenGLIndexBuffer(EBufferUsage usage) 
		: EIndexBuffer(usage), m_Count(0)
	{
		IN_RENDER_S({
			glCall(glGenBuffers(1, &self->m_RendererID));	
		})
	}

	EOpenGLIndexBuffer::EOpenGLIndexBuffer(ESharedBuffer data, EBufferUsage usage)
		: EIndexBuffer(usage), m_Count(data.GetElementCount())
	{
		IN_RENDER_S2(data, usage, {
				glCall(glGenBuffers(1, &self->m_RendererID));
				glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->m_RendererID));
                glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.GetSizeInByte(), data.Data(), GetGLBufferUsage(usage)));
			})
	}

	EOpenGLIndexBuffer::~EOpenGLIndexBuffer()
	{
		IN_RENDER_S({
				glCall(glDeleteBuffers(1, &self->m_RendererID));
			})
	}

	void EOpenGLIndexBuffer::Bind() const
	{
		IN_RENDER_S({
				glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->m_RendererID));
			})
	}

	void EOpenGLIndexBuffer::Unbind() const
	{
#ifdef IN_DEBUG
		IN_RENDER({
				glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
			})
#endif
	}
	
	void EOpenGLIndexBuffer::SetData(u8* data, u32 size_in_bytes, u32 element_size) 
	{
		Bind();

		Engine::ESharedBuffer sharedBuffer;
		if (element_size == 1) {
			sharedBuffer.InitWith<u8>(data, size_in_bytes);
		} else if (element_size == 2) {
			sharedBuffer.InitWith<u16>(data, size_in_bytes);
		} else if (element_size == 4) {
			sharedBuffer.InitWith<u32>(data, size_in_bytes);
		} else {
			E_WARN("Cant set vertex data with given element size. Supported sizes are 1, 2 and 4 bytes");
		}

		IN_RENDER_S1(data, {
			glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.GetSizeInByte(), data.Data(), GetGLBufferUsage(self->fBufferUsage)));
		})
	}
	
EOpenGLVertexArray::EOpenGLVertexArray()
{
	fIndexBuffer = nullptr;
	IN_RENDER_S({
		glCall(glGenVertexArrays(1, &self->fRendererID));
		glCall(glBindVertexArray(self->fRendererID);) 
	})
}

EOpenGLVertexArray::~EOpenGLVertexArray()
{		
	fVertexBuffers.clear();

	IN_RENDER_S({
		glCall(glDeleteVertexArrays(1, &self->fRendererID));
	})
}

void EOpenGLVertexArray::Bind() const
{
	IN_RENDER_S({
		glCall(glBindVertexArray(self->fRendererID));
	});

	for (const ERef<EVertexBuffer>& vb : fVertexBuffers)
	{
		vb->Bind();
	}
    if (fIndexBuffer)
    {
        fIndexBuffer->Bind();
    }
}

void EOpenGLVertexArray::Unbind() const
{
	IN_RENDER({
		glCall(glBindVertexArray(0));
	});
}


void EOpenGLVertexArray::AddVertexBuffer(const ERef<EVertexBuffer>& vertexBuffer)
{
	if (vertexBuffer->GetLayout().GetElements().size() == 0)
	{	
		std::cout << "No Vertex Layout set for the buffer" << std::endl;
		return;
	}

	fVertexBuffers.push_back(vertexBuffer);
	Bind();
	vertexBuffer->Bind();

	const auto layout = vertexBuffer->GetLayout();
	u32 stride = layout.GetStride();
	for (const auto& element : layout)
	{
		IN_RENDER_S2(element, stride, {
			glCall(glEnableVertexAttribArray(self->fVertexBufferIndex));
			glCall(glVertexAttribPointer(self->fVertexBufferIndex,
						element.GetComponentCount(),
						ShaderDataTypeToOpenGLType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						stride, (const void*)element.Offset));
			self->fVertexBufferIndex++;
			
		});
	}
}

const ERef<EIndexBuffer>& EOpenGLVertexArray::GetIndexBuffer() const
{
	return fIndexBuffer;
}

void EOpenGLVertexArray::SetIndexBuffer(const ERef<EIndexBuffer>& indexBuffer)
{
	fIndexBuffer = indexBuffer;

	IN_RENDER_S({
		glBindVertexArray(self->fRendererID);
		self->fIndexBuffer->Bind();
	});
}



EOpenGLFrameBuffer::EOpenGLFrameBuffer(u32 width, u32 height, EFramebufferFormat format)
	: m_Format(format), m_Width(0), m_Height(0), m_RendererID(0), m_DepthAttachment(0), m_ColorAttachment(0)
{
	IN_RENDER_S({
		
	})
	Resize(width, height);
}

EOpenGLFrameBuffer::~EOpenGLFrameBuffer()
{
	IN_RENDER_S({
		glCall(glDeleteFramebuffers(1, &self->m_RendererID));
		glCall(glDeleteTextures(1, &self->m_DepthAttachment));
		glCall(glDeleteTextures(1, &self->m_ColorAttachment));
	})
}

void EOpenGLFrameBuffer::Resize(u32 width, u32 height)
{
	if (width == m_Width && height == m_Height) return;

	m_Width = width;
	m_Height = height;


	IN_RENDER_S({

		if (self->m_RendererID)
		{
			glCall(glDeleteFramebuffers(1, &self->m_RendererID));
		}
		
		if (self->m_DepthAttachment)
		{
			glCall(glDeleteTextures(1, &self->m_DepthAttachment));
		}
		
		if (self->m_ColorAttachment) 
		{
			glCall(glDeleteTextures(1, &self->m_ColorAttachment));
		}


		glCall(glGenFramebuffers(1, &self->m_RendererID));
		glCall(glBindFramebuffer(GL_FRAMEBUFFER, self->m_RendererID));

		

		glCall(glGenTextures(1, &self->m_ColorAttachment));
		glCall(glBindTexture(GL_TEXTURE_2D, self->m_ColorAttachment));

		if (self->m_Format == EFramebufferFormat::RGBA16F)
		{
			glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, self->m_Width, self->m_Height, 0, GL_RGBA, GL_FLOAT, nullptr));
		}
		else if (self->m_Format == EFramebufferFormat::RGBA8)
		{
			glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, self->m_Width, self->m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
		}

		glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        glCall(glBindTexture(GL_TEXTURE_2D, 0));
        
        
		glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, self->m_ColorAttachment, 0));
        GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			std::cout << "GL_ERROR: " << err << std::endl;
		}

		glCall(glGenTextures(1, &self->m_DepthAttachment));
		glCall(glBindTexture(GL_TEXTURE_2D, self->m_DepthAttachment));
		glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, self->m_Width, self->m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL));

		glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, self->m_DepthAttachment, 0));

        GLenum frameBufferResult = GL_FRAMEBUFFER_COMPLETE;
		if ((frameBufferResult = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Depth Framebuffer is incomplete!" << frameBufferResult << std::endl;
		}
			

		glCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	})
}

void EOpenGLFrameBuffer::Bind() const
{
	IN_RENDER_S({
			glCall(glBindFramebuffer(GL_FRAMEBUFFER, self->m_RendererID));
			glCall(glViewport(0, 0, self->m_Width, self->m_Height));
		})
}

void EOpenGLFrameBuffer::Unbind() const
{
	IN_RENDER({
			glCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	})
}

void EOpenGLFrameBuffer::BindTexture(u32 slot) const
{
	IN_RENDER_S1(slot, {
			glCall(glActiveTexture(GL_TEXTURE0 + slot));
			glCall(glBindTexture(GL_TEXTURE_2D, self->m_ColorAttachment));
		})
}



}
