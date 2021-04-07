#include "graphics_wrapper.h"
#include "graphics_renderer.h"

using namespace Graphics;
using namespace Renderer;

std::vector<RMesh::Vertex> vertices = {
    {{-0.5f,-0.5f,0.0f}},
    {{ 0.5f,-0.5f,0.0f}},
    {{ 0.5f, 0.5f,0.0f}},
    {{-0.5f, 0.5f,0.0f}},
};

std::vector<unsigned int> indices = {
    0, 1, 2,
    2, 3, 0  
};


static GContext* mainContext = nullptr;
static Renderer::RMesh* mesh = nullptr;
static GFrameBuffer* frameBuffer = nullptr;

void Init(GContext* context)
{
    mainContext = context;
    frameBuffer = Wrapper::CreateFrameBuffer(10, 10, GFrameBufferFormat::RGBA8);

    mesh = new Renderer::RMesh();
    mesh->SetData(vertices, indices);
}

void CleanUp()
{
    delete frameBuffer;
    delete mesh;
}

void Render();
void RenderImGui();

int main()
{
    Wrapper::RunApplicationLoop(&Init, &Render, &RenderImGui, &CleanUp);
}

void Render()
{
    Renderer::RRenderer3D renderer(mainContext);

    renderer.Begin(frameBuffer);
    renderer.Submit(mesh);
    renderer.End();
}

void RenderImGui()
{
    ImGui::Begin("MyMesh");
    ImVec2 contentReg = ImGui::GetContentRegionAvail();
    frameBuffer->Resize(contentReg.x, contentReg.y, GFrameBufferFormat::RGBA8);
    ImGui::Image((ImTextureID)(unsigned long)frameBuffer->GetColorAttachment(), contentReg);
    ImGui::End();
}