#include "engine.h"
#include "prefix_editor.h"


namespace Engine {

	ERef<ETexture2D> ETexture2D::Create(const EString& name, ETextureFormat format, u32 width, u32 height)
	{
		//TODO: ADD SERRT
		//IN_CORE_ASSERT((bool)ERenderContext, "No Renderer selected!");
		switch (ERenderContext::Renderer)
		{
		case ERenderingType::OpenGL: 	return ERef<EOpenGLTexture2D>(new EOpenGLTexture2D(name, format, width, height));
		case ERenderingType::None:		break;
		}
		return nullptr;
	}

	ERef<ETextureCube> ETextureCube::Create(const EString& path)
	{
		//TODO: ADD SERRT
		//IN_CORE_ASSERT((bool)ERenderContext, "No Renderer selected!");
		switch (ERenderContext::Renderer)
		{
		case ERenderingType::OpenGL: 	return ERef<EOpenGLTextureCube>(new EOpenGLTextureCube(path));
		case ERenderingType::None:		break;
		}
		return nullptr;
	}


	/*
#define STB_IMAGE_IMPLEMENTATION
#include "util/stb_image.h"
	
	ERef<ETexture2D> Engine_LoadTextureFromFileBuffer(EString name, ESharedBuffer fileBuffer) 
	{
		int width, height, channels;
		std::cout << "Loading texture " << name << std::endl;

		stbi_set_flip_vertically_on_load(false);
		stbi_uc* imageData = stbi_load_from_memory(fileBuffer.Data<const stbi_uc>(), fileBuffer.GetSizeInByte(), &width, &height, &channels, STBI_rgb_alpha);

		ERef<ETexture2D> result = ETexture2D::Create(name, ETextureFormat::RGBA, width, height);
		result->SetTextureData(imageData, width, height);
		return result;
	}*/

}
