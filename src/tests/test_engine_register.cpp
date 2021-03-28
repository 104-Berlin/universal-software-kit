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

TEST(RegisterTest, BooleanDataHandle)
{
    using namespace Engine;
    EFloatDataHandle emptyData("EmptyBoolean");
    EXPECT_EQ(emptyData, false);

    EFloatDataHandle defaultValueData("DataWithDefaultValue", true);
    EXPECT_TRUE(defaultValueData.GetValue());
    defaultValueData.SetValue(false);
    EXPECT_FALSE(defaultValueData.GetValue());
    defaultValueData = true;
    EXPECT_TRUE(((bool)defaultValueData));
}

TEST(RegisterTest, StructureDataHandle)
{
    using namespace Engine;

    EStructureDataHandle structureHandle("MyStruct");
    structureHandle.AddField({"MyInteger", EDataType::INTEGER}, 20);
    structureHandle.AddField({"MyFloat", EDataType::FLOAT}, 3.5);
    structureHandle.AddField({"MyBoolean", EDataType::BOOLEAN}, true);

    EXPECT_TRUE(structureHandle.GetFieldAt("MyInteger"));
    EXPECT_FALSE(structureHandle.GetFieldAt("WRONG FIELD NAME"));

    ERef<EIntegerDataHandle> integerHandle = structureHandle.GetFieldAt<EIntegerDataHandle>("MyInteger");
    ERef<EFloatDataHandle> wrongFloatHandle = structureHandle.GetFieldAt<EFloatDataHandle>("MyInteger");
    ERef<EFloatDataHandle> floatHandle = structureHandle.GetFieldAt<EFloatDataHandle>("MyFloat");
    ERef<EBooleanDataHandle> boolHandle = structureHandle.GetFieldAt<EBooleanDataHandle>("MyBoolean");

    EXPECT_EQ(wrongFloatHandle, nullptr);
    EXPECT_NE(integerHandle, nullptr);
    EXPECT_NE(floatHandle, nullptr);
    EXPECT_NE(boolHandle, nullptr);

    if (integerHandle && boolHandle && floatHandle)
    {
        EXPECT_EQ(integerHandle->GetValue(), 20);
        EXPECT_EQ(floatHandle->GetValue(), 3.5);
        EXPECT_EQ(boolHandle->GetValue(), true);
    }
}