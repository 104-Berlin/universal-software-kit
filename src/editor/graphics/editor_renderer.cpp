#include "engine.h"
#include "prefix_editor.h"

namespace Editor {


const char* vertex_shader_src = ""
"#version 330 core\n"

"layout(location = 0) in vec3 position;\n"
"layout(location = 1) in vec3 normal;\n"
"layout(location = 2) in vec2 texCoord;\n"

"uniform mat4 vp_matrix = mat4(1.0);\n"
"uniform mat4 ml_matrix = mat4(1.0);\n"

"out vec2 UVS;\n"
"out vec3 TransNormals;\n"

"void main()\n"
"{\n"

	"vec4 worldPosition = ml_matrix * vec4(position, 1.0);\n"
    "gl_Position = vp_matrix * worldPosition;\n"
    "UVS = texCoord;\n"
    "TransNormals = normal;\n"
"}\n"
"";

const char* fragment_shader_src = ""
"#version 330 core\n"

"in vec2 UVS;\n"
"in vec3 TransNormals;\n"

"out vec4 FinalColor;\n"

"void main()\n"
"{\n"
    "FinalColor = vec4((TransNormals.x + 1) / 2, (TransNormals.y + 1) / 2, (TransNormals.z + 1) / 2, 1.0);\n"
"}\n"
"";

	void CreateDefaults()
	{
		/*Shader::DefaultShader = Shader::Create(vertexSource, fragmentSource);
		Material::DefaultMaterial = std::shared_ptr<Material>(new Material(Shader::DefaultShader));*/
	}

	void ERenderer::Init()
	{
		Get().fShader = EShader::Create(vertex_shader_src, fragment_shader_src);

		CreateDefaults();
	}
	
	ERenderer& ERenderer::Get()
	{
		static ERenderer renderer;
		return renderer;
	}
	
	void ERenderer::CleanUp() 
	{
		Get().fShader = nullptr;
	}
	
	void ERenderer::RenderVertexArray(const ERef<EVertexArray>& vertexArray) 
	{
		vertexArray->Bind();

		u32 indexCount = vertexArray->GetIndexBuffer()->GetCount();
		IN_RENDER1(indexCount, {
			glCall(glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, NULL));
		})
	}

	void ERenderer::Begin(const ERef<ECamera>& camera, const EMat4& viewMatrix, const ELightMap& lightMap, u32 width, u32 height)
	{
		IN_RENDER2(width, height, {
			glCall(glViewport(0, 0, width, height));
			//glCall(glEnable(GL_CULL_FACE));
        	glCall(glEnable(GL_DEPTH_TEST));
        	glCall(glDisable(GL_SCISSOR_TEST));
		})
		ERenderContext::s_Instance->SetClearColor({ 1.0f, 0.6f, 0.6f, 1.0f });
		ERenderContext::s_Instance->Clear();

        Get().fViewProjectionMatrix = camera->GetProjectionMatrix() * viewMatrix;
		Get().fCameraPosition = EVec3();
		//Make that better
		Get().fLightMap.clear();
		Get().fLightMap = lightMap;
	}

	void ERenderer::Draw(const ERef<EVertexArray>& vertexArray, const EMat4& transform)
	{
		if (!vertexArray) { return; }
		if (!Get().fShader)
		{
			std::cout << "The renderer has no shader. To render to" << std::endl;
			return;
		}

		Get().fShader->Bind();
		Get().fShader->SetUniformMat4("vp_matrix", Get().fViewProjectionMatrix);
		Get().fShader->SetUniformMat4("ml_matrix", transform);

		RenderVertexArray(vertexArray);
	}

	void ERenderer::End()
	{
	}

/*
	void ERenderer::Draw(MeshInstance* mesh, MaterialInstance* material, const glm::mat4& modelMatrix)
	{
		IN_CORE_ASSERT(s_Instance, "No Renderer instance set!"); //Forgot to call Renderer::Init(); ?
		IN_CORE_ASSERT(mesh, "Mesh not valid");
		IN_CORE_ASSERT(s_Instance->fLightMap.size() > 0, "No lights set for the scene!");
		//IN_CORE_ASSERT(mesh->UsedMaterial, "Pls provide a Material for the model!");
		if (!material)
		{
			static bool warningShows = false;
			if (!warningShows)
				IN_CORE_WARN("Pls provide a Material fdor the model!");
			warningShows = true;
			return;
		}

		//Clean this up!

		std::weak_ptr<Shader> shader = material->GetShaderProgram();
		if (shader.expired()) return;

		shader.lock()->SetUniformBuffer("u_ViewProjectionMatrix", (byte*)&s_Instance->fViewProjectionMatrix[0][0], sizeof(float) * 4 * 4);
		shader.lock()->SetUniformBuffer("u_ModelMatrix", (byte*)&modelMatrix[0][0], sizeof(float) * 4 * 4);
		shader.lock()->SetUniformBuffer("lights", (byte*) &s_Instance->fLightMap[0], sizeof(Light));
		shader.lock()->SetUniformBuffer("u_CameraPosition", (byte*)&s_Instance->fCameraPosition, sizeof(EVec3));
		
		shader.lock()->SetUniform3f("lights.Direction", s_Instance->fLightMap[0].Direction * TO_RADIANS);
		material->Bind();

		mesh->GetVertexArray()->Bind();
		u32 vertexCount = mesh->GetVertexCount();
		IN_RENDER1(vertexCount, {
				RendererAPI::s_Instance->DrawIndexed(vertexCount);
			})
	}
*/
	void ERenderer::WaitAndRender()
	{
		Get().fCommandQueue.Execute();
	}

}
