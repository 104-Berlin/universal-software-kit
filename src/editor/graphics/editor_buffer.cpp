#include "engine.h"
#include "prefix_editor.h"

namespace Editor {

	EVertexBuffer::EVertexBuffer(EBufferUsage usage) 
		: fBufferUsage(usage)
	{
		
	}
	
	ERef<EVertexBuffer> EVertexBuffer::Create(EBufferUsage usage)
	{
		//TODO: Add ASSERT
		//IN_CORE_ASSERT((bool)ERenderContext, "No RenderAPI selected");
		switch (ERenderContext::Renderer)
		{
		case ERenderingType::OpenGL:	return ERef<EOpenGLVertexBuffer>(new EOpenGLVertexBuffer(usage));
		case ERenderingType::None:		break;
		}
		
		return nullptr;
	}
	
	ERef<EVertexBuffer> EVertexBuffer::Create(u8* data, u32 data_size)
	{
		//TODO: Add ASSERT
		//IN_CORE_ASSERT((bool)ERenderContext, "No RenderAPI selected");
		switch (ERenderContext::Renderer)
		{
		case ERenderingType::OpenGL:	return ERef<EOpenGLVertexBuffer>(new EOpenGLVertexBuffer(data, data_size));
		case ERenderingType::None:		break;
		}
		
		return nullptr;
	}
	
	EIndexBuffer::EIndexBuffer(EBufferUsage usage) 
		: fBufferUsage(usage)
	{
		
	}

	ERef<EIndexBuffer> EIndexBuffer::Create(EBufferUsage usage)
	{
		//TODO: Add ASSERT
		//IN_CORE_ASSERT((bool)ERenderContext, "No RenderAPI selected");
		switch (ERenderContext::Renderer)
		{
		case ERenderingType::OpenGL:	return ERef<EOpenGLIndexBuffer>(new EOpenGLIndexBuffer(usage));
		case ERenderingType::None:		break;
		}
		return nullptr;
	}

	ERef<EIndexBuffer> EIndexBuffer::Create(u8* data, u32 size_in_bytes, u32 element_size)
	{
		//TODO: Add ASSERT
		//IN_CORE_ASSERT((bool)ERenderContext, "No RenderAPI selected");
		switch (ERenderContext::Renderer)
		{
		case ERenderingType::OpenGL:	return ERef<EOpenGLIndexBuffer>(new EOpenGLIndexBuffer(data, size_in_bytes, element_size));
		case ERenderingType::None:		break;
		}
		return nullptr;
	}


	ERef<EVertexArray> EVertexArray::Create()
	{
		switch (ERenderContext::Renderer)
		{
		case ERenderingType::OpenGL: return ERef<EOpenGLVertexArray>(new EOpenGLVertexArray());
		case ERenderingType::None:	break;
		}
		return nullptr;
	}


	ERef<EFrameBuffer> EFrameBuffer::Create(u32 width, u32 height, EFramebufferFormat format)
	{
		switch (ERenderContext::Renderer)
		{
		case ERenderingType::OpenGL: return ERef<EOpenGLFrameBuffer>(new EOpenGLFrameBuffer(width, height, format));
		case ERenderingType::None:	break;
		}
		
		return nullptr;
	}

}
