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

    EScene* activeScene = extensionManager.GetActiveScene();

    /*EScene::Entity entity = activeScene->CreateEntity();
    activeScene->InsertComponent(entity, "ElectricalInfo");

    EXPECT_TRUE(activeScene->HasComponent(entity, "ElectricalInfo"));

    EStructProperty* property = activeScene->GetComponent(entity, "ElectricalInfo");
    EProperty* capactity = property->GetProperty("Capacity");
    EXPECT_NE(capactity, nullptr);
    EValueDescription* capDesc = capactity->GetDescription();
    EXPECT_EQ(capDesc->GetType(), EValueType::PRIMITIVE);
    EXPECT_STREQ(capDesc->GetId().c_str(), "double");*/
}