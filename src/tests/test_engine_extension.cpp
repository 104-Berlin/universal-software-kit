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

    EValueDescription electricalDsc = extensionManager.GetValueDescriptionById("Example2", "Electrical");

    EXPECT_TRUE(electricalDsc.Valid());

    if (electricalDsc.Valid())
    {
        EScene* activeScene = extensionManager.GetActiveScene();

        EScene::Entity entity = activeScene->CreateEntity();
        activeScene->InsertComponent(entity, electricalDsc);

        EXPECT_TRUE(activeScene->HasComponent(entity, electricalDsc));

        EStructProperty* property = activeScene->GetComponent(entity, electricalDsc);
        EProperty* typeProp = property->GetProperty("Type");
        EXPECT_NE(typeProp, nullptr);
        EValueDescription typeDsc = typeProp->GetDescription();
        EXPECT_EQ(typeDsc.GetType(), EValueType::ENUM);
        EXPECT_STREQ(typeDsc.GetId().c_str(), "ElectricalType");
    }

}