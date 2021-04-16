#include "editor.h"

using namespace Graphics;
using namespace Renderer;
using namespace Engine;

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
static EExtensionManager* extensionManager = new EExtensionManager();

void Init(GContext* context)
{
    mainContext = context;
    frameBuffer = Wrapper::CreateFrameBuffer(10, 10, GFrameBufferFormat::RGBA8);

    mesh = new Renderer::RMesh();
    mesh->SetData(vertices, indices);

    extensionManager->LoadExtension("libExample1.so");
}

void CleanUp()
{
    delete frameBuffer;
    delete mesh;
    delete extensionManager;
}

void Render();
void RenderImGui();

int main()
{
    Wrapper::RunApplicationLoop(&Init, &Render, &RenderImGui, &CleanUp, &Wrapper::SetImGuiContext);
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
    EVector<ERef<EUIPanel>> allPanels = extensionManager->GetRegisteres().UIRegister->GetAllItems();
    for (ERef<EUIPanel> panel : allPanels)
    {
        panel->Render();
    }

    ImGui::Begin("MyMesh");
    ImVec2 contentReg = ImGui::GetContentRegionAvail();
    frameBuffer->Resize(contentReg.x, contentReg.y, GFrameBufferFormat::RGBA8);
    ImGui::Image((ImTextureID)(unsigned long)frameBuffer->GetColorAttachment(), contentReg);
    ImGui::End();
}