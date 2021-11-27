#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_shared.h>

using namespace Engine;

static EValueDescription dsc = EValueDescription::CreateStruct("SomeStruct", {{"Dub", DoubleDescription}});

TEST(SceneDelete, DeletingAScene)
{
    EDataBase* scene = new EDataBase();
    //scene->AddComponentCreateEventListener(dsc, [](EDataBase::Entity entity){});

    delete scene;
    scene = new EDataBase();

    delete scene;
}