#pragma once

namespace Editor {


	enum class ETextureFormat
	{
		None = 0,
		RGB = 1,
		RGBA = 2
	};

	class ETexture
	{
	public:
		virtual ~ETexture() {  }

		virtual void Bind(u32 slot) const = 0;
		
		virtual ETextureFormat GetFormat() const = 0;
		virtual u32 GetWidth() const = 0;
		virtual u32 GetHeight() const = 0;
		virtual u32 GetRendererID() const = 0;
		
		virtual void SetTextureData(byte* pixels, u32 width, u32 height) {}
	};

	class ETexture2D : public ETexture
	{
	public:
		static ERef<ETexture2D> Create(const EString& enginePath, ETextureFormat format, u32 width, u32 height);
	};

	class ETextureCube : public ETexture
	{
	public:
		static ERef<ETextureCube> Create(const EString& path);
	};


	// Load Function for resource manager
	//ERef<ETexture2D> Engine_LoadTextureFromFileBuffer(EString name, ESharedBuffer fileBuffer);

}
