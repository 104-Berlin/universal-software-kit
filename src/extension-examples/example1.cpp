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

static float matrix[16] =
{ 1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f };


static const float identityMatrix[16] =
{ 1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f };


static float cameraView[16] =
{ 1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f };

static float cameraProjection[16];

static size_t selectedIndex = 0;
static size_t selectedVertex = 0;

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
            ImGui::PushID(vertexId);
            ImGui::InputFloat3("Position", &vertex.Position.x);
            if (ImGui::Button("Select"))
            {   
                selectedVertex = vertexId;
            }
            if (selectedVertex == vertexId)
            {
                float matrixTranslation[3], matrixRotation[3], matrixScale[3];
                matrixTranslation[0] = vertex.Position.x;
                matrixTranslation[1] = vertex.Position.y;
                matrixTranslation[2] = vertex.Position.z;

                matrixRotation[0] = 0;
                matrixRotation[1] = 0;
                matrixRotation[2] = 0;

                matrixScale[0] = 1;
                matrixScale[1] = 1;
                matrixScale[2] = 1;

                //ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
                ImGuizmo::RecomposeMatrixFromComponents(&vertex.Position.x, matrixRotation, matrixScale, matrix);
                ImGuizmo::Manipulate(cameraView, cameraProjection, ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::LOCAL, matrix);

                ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
                vertex.Position.x = matrixTranslation[0];
                vertex.Position.y = matrixTranslation[1];
                vertex.Position.z = matrixTranslation[2];
            }
            ImGui::PopID();
            vertexId++;
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

void OrthoGraphic(const float l, float r, float b, const float t, float zn, const float zf, float* m16)
{
   m16[0] = 2 / (r - l);
   m16[1] = 0.0f;
   m16[2] = 0.0f;
   m16[3] = 0.0f;
   m16[4] = 0.0f;
   m16[5] = 2 / (t - b);
   m16[6] = 0.0f;
   m16[7] = 0.0f;
   m16[8] = 0.0f;
   m16[9] = 0.0f;
   m16[10] = 1.0f / (zf - zn);
   m16[11] = 0.0f;
   m16[12] = (l + r) / (l - r);
   m16[13] = (t + b) / (b - t);
   m16[14] = zn / (zn - zf);
   m16[15] = 1.0f;
}


void RenderViewport(Graphics::GContext* context, Graphics::GFrameBuffer* frameBuffer)
{
    ImGuiIO& io = ImGui::GetIO();
    float viewHeight = 1.0f * io.DisplaySize.y / io.DisplaySize.x;
    OrthoGraphic(-1.0f, 1.0f, 1.0, -1.0, 1000.f, -1000.f, cameraProjection);

    float windowWidth = (float)ImGui::GetWindowWidth();
    float windowHeight = (float)ImGui::GetWindowHeight();
    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
    float viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
    float viewManipulateTop = ImGui::GetWindowPos().y;

    ImGuizmo::SetDrawlist();

    //ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
    //ImGuizmo::DrawCubes(cameraView, cameraProjection, &objectMatrix[0][0], 1);

    //ImGuizmo::ViewManipulate(cameraView, 10, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);


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