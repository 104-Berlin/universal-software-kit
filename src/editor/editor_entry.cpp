#include "editor.h"

int main(int argc, char** argv)
{
    for (int i = 0; i < argc; i++)
    {
        printf("%s\n", argv[i]);
    }

    Engine::shared::StaticSharedContext::Start();
    {
        Editor::EApplication app;
        app.Start(argc == 2 ? argv[1] : "");
    }
    Engine::shared::StaticSharedContext::Stop();
}
