#include "engine.h"
#include "prefix_editor.h"

namespace Editor {

	static GLenum InfinitToOpenGLTextureFormat(ETextureFormat format)
	{
		switch (format)
		{
		case ETextureFormat::RGB:     return GL_RGB;
		case ETextureFormat::RGBA:    return GL_RGBA;
		case ETextureFormat::None:	  break;
		}
		return 0;
	}

	static size_t GetTextureFormatPixelSizeInBytes(ETextureFormat format)
	{
		switch (format)
		{
		case ETextureFormat::RGB:     return 3;
		case ETextureFormat::RGBA:    return 4;
		case ETextureFormat::None:	  break;
		}
		return 0;
	}

	static int CalculateMipMapCount(int width, int height)
	{
		int levels = 1;
		while ((width | height) >> levels) {
			levels++;
		}
		return levels;
	}

	EOpenGLTexture2D::EOpenGLTexture2D(ETextureFormat format, u32 width, u32 height)
		: fFormat(format), fWidth(width), fHeight(height), fRendererID(0)
	{
		IN_RENDER_S({
			glCall(glGenTextures(1, &self->fRendererID));
			glCall(glBindTexture(GL_TEXTURE_2D, self->fRendererID));

			glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			glCall(glTexImage2D(GL_TEXTURE_2D, 0, InfinitToOpenGLTextureFormat(self->fFormat), self->fWidth, self->fHeight, 0, InfinitToOpenGLTextureFormat(self->fFormat), GL_UNSIGNED_BYTE, NULL));
			glCall(glGenerateMipmap(GL_TEXTURE_2D));

			glCall(glBindTexture(GL_TEXTURE_2D, 0));
		})
	}

	EOpenGLTexture2D::~EOpenGLTexture2D()
	{
		IN_RENDER_S({
				glCall(glDeleteTextures(1, &self->fRendererID));
			})
	}

	void EOpenGLTexture2D::Bind(u32 slot) const
	{
		IN_RENDER_S1(slot, {
				glCall(glActiveTexture(GL_TEXTURE0 + slot));
				glCall(glBindTexture(GL_TEXTURE_2D, self->fRendererID));
			})

	}
	
	void EOpenGLTexture2D::SetTextureData(byte* pixels, u32 width, u32 height) 
	{
		Engine::
		byte* copy_array = new byte[width * height * GetTextureFormatPixelSizeInBytes(fFormat)];
		memcpy(copy_array, pixels, width * height * GetTextureFormatPixelSizeInBytes(fFormat));
		IN_RENDER_S3(copy_array, width, height, {
			glCall(glBindTexture(GL_TEXTURE_2D, self->fRendererID));
			glCall(glTexImage2D(GL_TEXTURE_2D, 0, InfinitToOpenGLTextureFormat(self->fFormat), width, height, 0, InfinitToOpenGLTextureFormat(self->fFormat), GL_UNSIGNED_BYTE, copy_array));
			delete[] copy_array;
		})
	}

//REMOVE srgb??
	EOpenGLTextureCube::EOpenGLTextureCube(const EString& path)
		: ETextureCube(path), fWidth(0), fHeight(0)
	{
	}

	EOpenGLTextureCube::~EOpenGLTextureCube()
	{
		IN_RENDER_S({
				glCall(glDeleteTextures(1, &self->fRendererID));
			})
	}

	void EOpenGLTextureCube::Bind(u32 slot) const
	{
		IN_RENDER_S1(slot, {
				glCall(glActiveTexture(GL_TEXTURE0 + slot));
				glCall(glBindTexture(GL_TEXTURE_CUBE_MAP, self->fRendererID));
			})
	}

}
