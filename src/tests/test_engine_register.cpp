#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_shared.h>

TEST(RegisterTest, DataHandle)
{
    using namespace Engine;
    EIntegerDataHandle emptyData("EmptyInteger");
    EXPECT_EQ(emptyData, 0);

    EIntegerDataHandle defaultValueData("DataWithDefaultValue", 20);
    EXPECT_EQ(defaultValueData.GetValue(), 20);
    defaultValueData.SetValue(40);
    EXPECT_EQ(defaultValueData.GetValue(), 40);
    defaultValueData = 10;
    EXPECT_EQ(((i32)defaultValueData), 10);
}

TEST(RegisterTest, StructureDataHandle)
{
    using namespace Engine;

    EStructureDataHandle structureHandle("MyStruct");
    structureHandle.AddField({"MyInteger", EDataType::INTEGER}, 20);

    EXPECT_TRUE(structureHandle.GetFieldAt("MyInteger"));

    ERef<EIntegerDataHandle> integerHandle = structureHandle.GetFieldAt<EIntegerDataHandle>("MyInteger");
    EXPECT_NE(integerHandle, nullptr);
    if (integerHandle)
    {
        EXPECT_EQ(integerHandle->GetValue(), 20);
    }
}