#include "editor_extension.h"

using namespace Engine;

class ViewportPanel : public EUIPanel
{
public:
    ViewportPanel(const EString& name = "Viewport-Panel")
        : EUIPanel(name)
    {}

    virtual bool OnRender() override
    {
        ImVec2 windowSize = ImGui::GetWindowSize();
        
        return true;
    }
};


APP_ENTRY
{
    
}