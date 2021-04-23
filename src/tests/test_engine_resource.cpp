#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_shared.h>

using namespace Engine;

TEST(Resources, BasicManager)
{
    struct MyResource
    {
        int SomeValue;
        EString someString;
    };

    EResourceManager resourceManager;
    resourceManager.AddOrUpdateResource("SomeResource", MyResource{20, "Hello World"});

    const MyResource* foundResource = resourceManager.GetResource<MyResource>("SomeResource");
    EXPECT_NE(foundResource, nullptr);
    EXPECT_EQ(foundResource->SomeValue, 20);
    EXPECT_STREQ(foundResource->someString.c_str(), "Hello World");
}