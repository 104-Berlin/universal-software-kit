#include "editor_extension.h"

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

std::vector<Graphics::GMesh::Vertex> planeVertices = {
    {{-1000.0f, -10.0f, -1000.0f}},
    {{ 1000.0f, -10.0f, -1000.0f}},
    {{ 1000.0f, -10.0f,  1000.0f}},
    {{-1000.0f, -10.0f,  1000.0f}},
};

std::vector<unsigned int> planeIndices = {
    0, 1, 2, 2, 3, 0
};

std::vector<Graphics::GMesh::Vertex> vertices = {
    {{-50.0f,-50.0f, -1.0f}},
    {{ 50.0f,-50.0f, -1.0f}},
    {{ 50.0f, 50.0f, -1.0f}},
    {{-50.0f, 50.0f, -1.0f}},
};

std::vector<unsigned int> indices = {
    0, 1, 2
};

static Graphics::GMesh* mesh = nullptr;
static Graphics::GMesh* planeMesh = nullptr;

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

static EWeakRef<Engine::EUIViewport> viewport;

class TestUiField : public Engine::EUIField
{
public:
    std::vector<Graphics::GMesh::Vertex> fVertices;
    std::vector<unsigned int> fIndices;
public:
    TestUiField()
        : Engine::EUIField("TestField"), fVertices(vertices), fIndices(indices)
    {

    }

    virtual bool OnRender()
    {
        size_t vertexId = 0;
        for (Graphics::GMesh::Vertex& vertex : fVertices)
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
        if (viewport.expired()) { return false; }
        glm::vec3 currentCamPos = viewport.lock()->GetCamera().GetPosition();
        ImGui::DragFloat3("Camera Position", &currentCamPos.x);
        viewport.lock()->GetCamera().SetPosition(currentCamPos);

        ImGui::Text("Camera Forward: (%f, %f, %f)", viewport.lock()->GetCamera().GetForward().x, viewport.lock()->GetCamera().GetForward().y, viewport.lock()->GetCamera().GetForward().z);
        ImGui::Text("Camera UP: (%f, %f, %f)", viewport.lock()->GetCamera().GetUp().x, viewport.lock()->GetCamera().GetUp().y, viewport.lock()->GetCamera().GetUp().z);
        ImGui::Text("Camera Right: (%f, %f, %f)", viewport.lock()->GetCamera().GetRight().x, viewport.lock()->GetCamera().GetRight().y, viewport.lock()->GetCamera().GetRight().z);

        ImGui::Spacing();
        ImGui::DragFloat("Camera rotation devider", &cameraRotationDevider);
        ImGui::DragFloat("Camera rotation devider", &cameraSpeedDevider);
        return true;
    }
};

static ERef<TestUiField> testUiField = nullptr;


APP_ENTRY
{
    ERef<Engine::EUIPanel> uiPanel = EMakeRef<Engine::EUIPanel>("First panel");
    viewport = std::dynamic_pointer_cast<Engine::EUIViewport>(uiPanel->AddChild(EMakeRef<Engine::EUIViewport>()));
    mesh = new Graphics::GMesh();
    mesh->SetData(vertices, indices);
    viewport.lock()->GetScene().Add(mesh);


    ERef<Engine::EUIPanel> vertexChanginngPanel = EMakeRef<Engine::EUIPanel>("Change The vertices");
    testUiField = EMakeRef<TestUiField>();
    vertexChanginngPanel->AddChild(testUiField);

    ERef<Engine::EUIPanel> infoPanel = EMakeRef<Engine::EUIPanel>("Info Panel");
    infoPanel->AddChild(EMakeRef<InfoPanel>());


    info.PanelRegister->RegisterItem(extensionName, uiPanel);
    info.PanelRegister->RegisterItem(extensionName, vertexChanginngPanel);
    info.PanelRegister->RegisterItem(extensionName, infoPanel);
}

EXT_ENTRY
{
    E_INFO(EString("Initiliazing ") + extensionName);
}
