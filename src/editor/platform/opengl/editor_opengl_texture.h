#pragma once

namespace Editor {

	class EOpenGLTexture2D : public ETexture2D
	{
	public:
		EOpenGLTexture2D(ETextureFormat format, u32 width, u32 height);

		virtual ~EOpenGLTexture2D();

		virtual u32 GetRendererID() const override { return fRendererID; }
		virtual void Bind(u32 slot) const override;

		virtual ETextureFormat GetFormat() const override { return fFormat; }
		virtual u32 GetHeight() const override { return fHeight; }
		virtual u32 GetWidth() const override { return fWidth; }

		virtual void SetTextureData(byte* pixels, u32 width, u32 height) override;
	private:
		u32 fRendererID;
		ETextureFormat fFormat;
		u32 fWidth;
		u32 fHeight;
		byte* fImageData;
	};

	class EOpenGLTextureCube : public ETextureCube
	{
	public:
		EOpenGLTextureCube();

		virtual ~EOpenGLTextureCube();

		virtual u32 GetRendererID() const override { return fRendererID; }
		virtual void Bind(u32 slot) const override;

		virtual ETextureFormat GetFormat() const override { return fFormat; }
		virtual u32 GetHeight() const override { return fHeight; }
		virtual u32 GetWidth() const override { return fWidth; }
	private:
		u32 fRendererID;
		ETextureFormat fFormat;
		u32 fWidth;
		u32 fHeight;
		byte* fImageData;
	};

}
