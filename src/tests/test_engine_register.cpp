#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_shared.h>

TEST(RegisterTest, IntegerDataHandle)
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

TEST(RegisterTest, FloatDataHandle)
{
    using namespace Engine;
    EFloatDataHandle emptyData("EmptyFloat");
    EXPECT_EQ(emptyData, 0);

    EFloatDataHandle defaultValueData("DataWithDefaultValue", 2.4);
    EXPECT_FLOAT_EQ(defaultValueData.GetValue(), 2.4);
    defaultValueData.SetValue(5.2);
    EXPECT_FLOAT_EQ(defaultValueData.GetValue(), 5.2);
    defaultValueData = 2.22;
    EXPECT_FLOAT_EQ(((float)defaultValueData), 2.22);
}

TEST(RegisterTest, StructureDataHandle)
{
    using namespace Engine;

    EStructureDataHandle structureHandle("MyStruct");
    structureHandle.AddField({"MyInteger", EDataType::INTEGER}, 20);
    structureHandle.AddField({"MyInteger", EDataType::FLOAT}, 20);
    structureHandle.AddField({"MyInteger", EDataType::BOOLEAN}, 20);

    EXPECT_TRUE(structureHandle.GetFieldAt("MyInteger"));

    ERef<EIntegerDataHandle> integerHandle = structureHandle.GetFieldAt<EIntegerDataHandle>("MyInteger");
    EXPECT_NE(integerHandle, nullptr);
    if (integerHandle)
    {
        EXPECT_EQ(integerHandle->GetValue(), 20);
    }
}