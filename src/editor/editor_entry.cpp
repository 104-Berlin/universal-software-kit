#include "editor.h"

int main()
{
    Engine::shared::StaticSharedContext::Start();
    {
        Editor::EApplication app;
        app.Start();
    }
    Engine::shared::StaticSharedContext::Stop();
}
