#ifdef EXT_RENDERER_ENABLED
#include "engine_extension.h"

/**
 * {{-0.5f,-0.5f,0.0f}},
    {{ 0.5f,-0.5f,0.0f}},
    {{ 0.5f, 0.5f,0.0f}},
    {{-0.5f, 0.5f,0.0f}},

    {{2.0f,-0.5f,-0.5f}},
    {{2.0f,-0.5f, 0.5f}},
    {{2.0f, 0.5f, 0.5f}},
    {{2.0f, 0.5f,-0.5f}},
    */

std::vector<Renderer::RMesh::Vertex> planeVertices = {
    {{-1000.0f, -10.0f, -1000.0f}},
    {{ 1000.0f, -10.0f, -1000.0f}},
    {{ 1000.0f, -10.0f,  1000.0f}},
    {{-1000.0f, -10.0f,  1000.0f}},
};

std::vector<unsigned int> planeIndices = {
    0, 1, 2, 2, 3, 0
};

std::vector<Renderer::RMesh::Vertex> vertices = {
    {{-0.5f,-0.5f, 1.0f}},
    {{ 0.5f,-0.5f, 1.0f}},
    {{ 0.5f, 0.5f, 1.0f}},
    {{-0.5f, 0.5f, 1.0f}},
};

std::vector<unsigned int> indices = {
    0, 1, 2
};

static Renderer::RMesh* mesh = nullptr;
static Renderer::RMesh* planeMesh = nullptr;

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

static glm::mat4 cameraView;
static glm::mat4 cameraProjection;

static size_t selectedIndex = 0;
static size_t selectedVertex = 0;

static float cameraRotationDevider = 360.0f;
static float cameraSpeedDevider = 10.0f;

static Renderer::RCamera ViewportCamera(Renderer::ECameraMode::PERSPECTIVE);

class TestUiField : public Engine::EUIField
{
public:
    std::vector<Renderer::RMesh::Vertex> fVertices;
    std::vector<unsigned int> fIndices;
public:
    TestUiField()
        : Engine::EUIField("TestField"), fVertices(vertices), fIndices(indices)
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

                ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);
                ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD, matrix);

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

class InfoPanel : public Engine::EUIField
{
public:
    InfoPanel()
    : Engine::EUIField("InfoPanel") {}

    virtual bool OnRender() override
    {
        glm::vec3 currentCamPos = ViewportCamera.GetPosition();
        ImGui::DragFloat3("Camera Position", &currentCamPos.x);
        ViewportCamera.SetPosition(currentCamPos);

        ImGui::Text("Camera Forward: (%f, %f, %f)", ViewportCamera.GetForward().x, ViewportCamera.GetForward().y, ViewportCamera.GetForward().z);
        ImGui::Text("Camera UP: (%f, %f, %f)", ViewportCamera.GetUp().x, ViewportCamera.GetUp().y, ViewportCamera.GetUp().z);
        ImGui::Text("Camera Right: (%f, %f, %f)", ViewportCamera.GetRight().x, ViewportCamera.GetRight().y, ViewportCamera.GetRight().z);

        ImGui::Spacing();
        ImGui::DragFloat("Camera rotation devider", &cameraRotationDevider);
        ImGui::DragFloat("Camera rotation devider", &cameraSpeedDevider);
        return true;
    }
};

static ERef<TestUiField> testUiField = nullptr;

void RenderViewport(Graphics::GContext* context, Graphics::GFrameBuffer* frameBuffer)
{
    cameraProjection = ViewportCamera.GetProjectionMatrix(frameBuffer->GetWidth(), frameBuffer->GetHeight());
    cameraView = ViewportCamera.GetViewMatrix();
    float windowWidth = (float)ImGui::GetWindowWidth();
    float windowHeight = (float)ImGui::GetWindowHeight();
    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();
    ImVec2 leftButtonDelta = ImGui::GetMouseDragDelta(0, 0);
    ImGui::ResetMouseDragDelta(0);
    ImVec2 rightButtonDelta = ImGui::GetMouseDragDelta(1, 0);
    ImGui::ResetMouseDragDelta(1);

    if (!ImGuizmo::IsUsing() && ImGui::IsWindowFocused())
    {
        ViewportCamera.MoveForward(ImGui::GetIO().MouseWheel);
        if (ImGui::IsKeyDown(USK_KEY_LEFT_CONTROL))
        {
            ViewportCamera.MoveRight(-leftButtonDelta.x / 10.0f);
            ViewportCamera.TurnRight(-rightButtonDelta.x / 360.0f);
            ViewportCamera.MoveUp(leftButtonDelta.y / 10.0f);
            ViewportCamera.TurnUp(-rightButtonDelta.y / 360.0f);
        }
    }

    mesh->SetData(testUiField->fVertices, testUiField->fIndices);

    Renderer::RRenderer3D renderer(context);
    renderer.Begin(frameBuffer, &ViewportCamera);
    renderer.Submit(planeMesh);
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

    ERef<Engine::EUIPanel> infoPanel = EMakeRef<Engine::EUIPanel>("Info Panel");
    infoPanel->AddChild(EMakeRef<InfoPanel>());


    extension->UIRegister->RegisterItem(extensionName, uiPanel);
    extension->UIRegister->RegisterItem(extensionName, vertexChanginngPanel);
    extension->UIRegister->RegisterItem(extensionName, infoPanel);

    mesh = new Renderer::RMesh();
    mesh->SetData(vertices, indices);

    planeMesh = new Renderer::RMesh();
    planeMesh->SetData(planeVertices, planeIndices);
}

EXT_CLEANUP
{
    delete mesh;
    delete planeMesh;
}

#endif