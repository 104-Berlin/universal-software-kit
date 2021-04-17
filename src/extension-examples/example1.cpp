#include "engine_extension.h"


std::vector<Renderer::RMesh::Vertex> vertices = {
    {{-0.5f,-0.5f,0.0f}},
    {{ 0.5f,-0.5f,0.0f}},
    {{ 0.5f, 0.5f,0.0f}},
    {{-0.5f, 0.5f,0.0f}},
};

std::vector<unsigned int> indices = {
    0, 1, 2
};

static Renderer::RMesh* mesh = nullptr;

class TestUiField : public Engine::EUIField
{
public:
    TestUiField();

    virtual bool OnRender() override;
};

TestUiField::TestUiField() 
    : Engine::EUIField("UIField")
{
    
}

bool TestUiField::OnRender()
{
    ImGui::Button("SomeButton");
    return true;
}

void RenderViewport(Graphics::GContext* context, Graphics::GFrameBuffer* frameBuffer)
{
    Renderer::RRenderer3D renderer(context);
    renderer.Begin(frameBuffer);
    renderer.Submit(mesh);
    renderer.End();
}

EXT_ENTRY
{
    E_INFO(EString("Initiliazing ") + extensionName);
    ERef<Engine::EUIPanel> uiPanel = EMakeRef<Engine::EUIPanel>("First panel");
    ERef<Engine::EUIViewport> viewport = EMakeRef<Engine::EUIViewport>();
    viewport->SetRenderFunction(&RenderViewport);
    uiPanel->AddChild(viewport);

    extension->UIRegister->RegisterItem(extensionName, uiPanel);

    mesh = new Renderer::RMesh();
    mesh->SetData(vertices, indices);
}

EXT_CLEANUP
{
    delete mesh;
}