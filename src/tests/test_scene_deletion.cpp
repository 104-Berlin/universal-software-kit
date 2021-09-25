#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_shared.h>

using namespace Engine;

static EValueDescription dsc = EValueDescription::CreateStruct("SomeStruct", {{"Dub", DoubleDescription}});

TEST(SceneDelete, DeletingAScene)
{
    ERegister* scene = new ERegister();
    //scene->AddComponentCreateEventListener(dsc, [](ERegister::Entity entity){});

    delete scene;
    scene = new ERegister();

    delete scene;
}