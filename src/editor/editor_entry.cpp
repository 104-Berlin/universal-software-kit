#include "editor.h"

using namespace Graphics;
using namespace Renderer;
using namespace Engine;


static GContext* mainContext = nullptr;
static EExtensionManager* extensionManager = new EExtensionManager();

void Init(GContext* context)
{
    mainContext = context;
    extensionManager->LoadExtension("Example1.uex");

    intern::InitUI();
}

void CleanUp()
{
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
}

void RenderImGui()
{
    EVector<ERef<EUIPanel>> allPanels = extensionManager->GetRegisteres().UIRegister->GetAllItems();
    for (ERef<EUIPanel> panel : allPanels)
    {
        panel->Render();
    }
}