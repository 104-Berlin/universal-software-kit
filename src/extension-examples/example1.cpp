#ifdef EXT_RENDERER_ENABLED
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
    std::vector<Renderer::RMesh::Vertex> fVertices;
    std::vector<unsigned int> fIndices;
public:
    TestUiField()
        : Engine::EUIField("TestField")
    {

    }

    virtual bool OnRender()
    {
        size_t vertexId = 0;
        for (Renderer::RMesh::Vertex& vertex : fVertices)
        {
            ImGui::PushID(vertexId++);
            ImGui::InputFloat3("Position", &vertex.Position.x);
            ImGui::PopID();
        }
        if (ImGui::Button("+##vertex"))
        {
            fVertices.push_back({});
        }

        for (size_t i = 0; i < fIndices.size(); i += 3)
        {
            ImGui::PushID(i);
            ImGui::InputInt3("Triangle", (int*) &fIndices[i]);
            ImGui::PopID();
        }
        if (ImGui::Button("+##index"))
        {
            fIndices.push_back(0);
            fIndices.push_back(0);
            fIndices.push_back(0);
        }
        return true;
    }
};

static ERef<TestUiField> testUiField = nullptr;

void RenderViewport(Graphics::GContext* context, Graphics::GFrameBuffer* frameBuffer)
{
    mesh->SetData(testUiField->fVertices, testUiField->fIndices);

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



    ERef<Engine::EUIPanel> vertexChanginngPanel = EMakeRef<Engine::EUIPanel>("Change The vertices");
    testUiField = EMakeRef<TestUiField>();
    vertexChanginngPanel->AddChild(testUiField);


    extension->UIRegister->RegisterItem(extensionName, uiPanel);
    extension->UIRegister->RegisterItem(extensionName, vertexChanginngPanel);

    mesh = new Renderer::RMesh();
    mesh->SetData(vertices, indices);
}

EXT_CLEANUP
{
    delete mesh;
}

#endif