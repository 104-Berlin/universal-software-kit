#pragma once

namespace Editor {

	class ERenderer
	{
	public:
		static void Init();
		static void CleanUp();

		static void RenderVertexArray(const ERef<EVertexArray>& vertexArray);

		static void Begin(const ERef<ECamera>& camera, const EMat4& viewMatrix, const ELightMap& lights, u32 width, u32 height);
		static void Draw(const ERef<EVertexArray>& vertexArray, const EMat4& transform = EMat4(1.0f));
		static void Submit(ERenderCommand* command) { return Get().fCommandQueue.AddRenderCommand(command); }
		static void End();

		static ERenderer& Get();

		static void WaitAndRender();

	private:

		ERenderCommandQueue fCommandQueue;


		//TEMP
		ERef<EShader>		fShader;

		glm::mat4 			fViewProjectionMatrix;
		EVec3 				fCameraPosition;
		ELightMap 			fLightMap;
	};

}

#define IN_RENDER(code) E_RENDERCOMMAND(code)\
						ERenderer::Submit(new CONCATENATE(_RenderCommand_, __LINE__)());
#define IN_RENDER1(var, code) E_RENDERCOMMAND(var, code)\
						ERenderer::Submit(new CONCATENATE(_RenderCommand_, __LINE__)(var));
#define IN_RENDER2(var1, var2, code) E_RENDERCOMMAND(var1, var2, code)\
						ERenderer::Submit(new CONCATENATE(_RenderCommand_, __LINE__)(var1, var2));
#define IN_RENDER3(var1, var2, var3, code) E_RENDERCOMMAND(var1, var2, var3, code)\
						ERenderer::Submit(new CONCATENATE(_RenderCommand_, __LINE__)(var1, var2, var3));
#define IN_RENDER4(var1, var2, var3, var4, code) E_RENDERCOMMAND(var1, var2, var3, var4, code)\
						ERenderer::Submit(new CONCATENATE(_RenderCommand_, __LINE__)(var1, var2, var3, var4));

#define IN_RENDER_S(code) 		auto self = this;\
								E_RENDERCOMMAND(self, code)\
								ERenderer::Submit(new CONCATENATE(_RenderCommand_, __LINE__)(self));
#define IN_RENDER_S1(var, code) auto self = this;\
								E_RENDERCOMMAND(self, var, code)\
								ERenderer::Submit(new CONCATENATE(_RenderCommand_, __LINE__)(self, var));
#define IN_RENDER_S2(var1, var2, code) 	auto self = this;\
										E_RENDERCOMMAND(self, var1, var2, code)\
										ERenderer::Submit(new CONCATENATE(_RenderCommand_, __LINE__)(self, var1, var2));
#define IN_RENDER_S3(var1, var2, var3, code) 	auto self = this;\
												E_RENDERCOMMAND(self, var1, var2, var3, code)\
												ERenderer::Submit(new CONCATENATE(_RenderCommand_, __LINE__)(self, var1, var2, var3));
