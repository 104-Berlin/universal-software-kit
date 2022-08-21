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
    EDataBase* activeScene = new EDataBase();

    EExtensionManager extensionManager;
    extensionManager.LoadExtension("Example2.uex", false);

    EValueDescription electricalDsc = extensionManager.GetValueDescriptionById("Example2", "Electrical");

    EXPECT_TRUE(electricalDsc.Valid());

    if (electricalDsc.Valid())
    {
        EDataBase::Entity entity = activeScene->CreateEntity();
        activeScene->AddComponent(entity, electricalDsc);

        EXPECT_TRUE(activeScene->HasComponent(entity, electricalDsc));

        ERef<EStructProperty> property = std::dynamic_pointer_cast<EStructProperty>(activeScene->GetComponent(entity, electricalDsc).lock());
        ERef<EProperty> typeProp = property->GetProperty("Type");
        EXPECT_NE(typeProp, nullptr);
        EValueDescription typeDsc = typeProp->GetDescription();
        EXPECT_EQ(typeDsc.GetType(), EValueType::ENUM);
        EXPECT_STREQ(typeDsc.GetId().c_str(), "ElectricalType");
    }

}