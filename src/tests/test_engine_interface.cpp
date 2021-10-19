#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_interface.h>

using namespace Engine;

E_STORAGE_STRUCT(SomeTestType,
    (EString, SomeString)
)

TEST(Interface, CheckStartup)
{
    shared::StaticSharedContext::Start();

    EXPECT_TRUE(shared::StaticSharedContext::instance().GetRegisterConnection().IsConnected());

    // Create component
    shared::CreateEntity();
    shared::CreateComponent(SomeTestType::_dsc, 1);

    SomeTestType foundValue;
    bool foundSomeType = shared::GetValue<SomeTestType>(1, &foundValue);
    EXPECT_TRUE(foundSomeType);

    

    shared::StaticSharedContext::Stop();
}
