#include <gtest/gtest.h>
#include <engine.h>
#include <engine_extension.h>

#define EXTENSION_NAME "Exmaple1-Extension"

using namespace Engine;

TEST(ExtensionTest, Basics)
{
}

TEST(ExtensionTest, LoadExtension)
{
    EExtensionManager extensionManager;
    extensionManager.LoadExtension("Example2.uex");

    ERef<EValueDescription> electricalDsc = extensionManager.GetValueDescriptionById("Example2", "Electrical");

    EXPECT_NE(electricalDsc, nullptr);

    if (electricalDsc)
    {
        EScene* activeScene = extensionManager.GetActiveScene();

        EScene::Entity entity = activeScene->CreateEntity();
        activeScene->InsertComponent(entity, electricalDsc);

        EXPECT_TRUE(activeScene->HasComponent(entity, electricalDsc));

        EStructProperty* property = activeScene->GetComponent(entity, electricalDsc);
        EProperty* capactity = property->GetProperty("Capacity");
        EXPECT_NE(capactity, nullptr);
        ERef<EValueDescription> capDesc = capactity->GetDescription();
        EXPECT_EQ(capDesc->GetType(), EValueType::PRIMITIVE);
        EXPECT_STREQ(capDesc->GetId().c_str(), "double");
    }

}