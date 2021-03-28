#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_shared.h>

Engine::EStructureDescription TestStruct({"TestStruct", Engine::EDataType::STRUCTURE}, {
                        Engine::EStructureDescription({"MyInteger", Engine::EDataType::INTEGER}),
                        Engine::EStructureDescription({"MyFloat", Engine::EDataType::FLOAT}),
                        Engine::EStructureDescription({"MyBool", Engine::EDataType::BOOLEAN}),
                        Engine::EStructureDescription({"MyString", Engine::EDataType::STRING})
});


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
    EBooleanDataHandle emptyData("EmptyBoolean");
    EXPECT_EQ(emptyData, false);

    EBooleanDataHandle defaultValueData("DataWithDefaultValue", true);
    EXPECT_TRUE(defaultValueData.GetValue());
    defaultValueData.SetValue(false);
    EXPECT_FALSE(defaultValueData.GetValue());
    defaultValueData = true;
    EXPECT_TRUE(((bool)defaultValueData));
}

TEST(RegisterTest, StringDataHandle)
{
    using namespace Engine;
    EStringDataHandle emptyData("EmptyString");
    EXPECT_STREQ(emptyData.GetValue().c_str(), "");

    EStringDataHandle defaultValueData("DataWithDefaultValue", "Default");
    EXPECT_STREQ(defaultValueData.GetValue().c_str(), "Default");
    defaultValueData.SetValue("Second");
    EXPECT_STREQ(defaultValueData.GetValue().c_str(), "Second");
    defaultValueData = "Third";
    EXPECT_STREQ(((EString)defaultValueData).c_str(), "Third");
}

TEST(RegisterTest, StructureDataHandle)
{
    using namespace Engine;

    EStructureDataHandle structureHandle("MyStruct", TestStruct);
    structureHandle.AddField<int>("MyInteger", 20);
    structureHandle.AddField<float>("MyFloat", 3.5);
    structureHandle.AddField<EBooleanDataHandle>("MyBoolean", true);
    structureHandle.AddField<EString>("MyString", "Hey you");

    EXPECT_EQ(structureHandle.GetFields().size(), 4);
    for (auto x : structureHandle) 
    {
        EXPECT_FALSE(x.first.empty());
    }

    EXPECT_EQ(structureHandle.GetFieldAt("WRONG FIELD NAME"), nullptr);
    EXPECT_NE(structureHandle.GetFieldAt("MyInteger"), nullptr);

    ERef<EIntegerDataHandle> integerHandle = structureHandle.GetFieldAt<EIntegerDataHandle>("MyInteger");
    ERef<EFloatDataHandle> wrongFloatHandle = structureHandle.GetFieldAt<EFloatDataHandle>("MyInteger");
    ERef<EFloatDataHandle> floatHandle = structureHandle.GetFieldAt<EFloatDataHandle>("MyFloat");
    ERef<EBooleanDataHandle> boolHandle = structureHandle.GetFieldAt<EBooleanDataHandle>("MyBoolean");
    ERef<EStringDataHandle> stringHandle = structureHandle.GetFieldAt<EStringDataHandle>("MyString");

    EXPECT_EQ(wrongFloatHandle, nullptr);
    EXPECT_NE(integerHandle, nullptr);
    EXPECT_NE(floatHandle, nullptr);
    EXPECT_NE(boolHandle, nullptr);
    EXPECT_NE(stringHandle, nullptr);

    const EStructureDataHandle& const_ref_handle = structureHandle;
    for (auto x : const_ref_handle) 
    {
        EXPECT_FALSE(x.first.empty());
    }
}