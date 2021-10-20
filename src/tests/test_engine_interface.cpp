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

    if (!shared::StaticSharedContext::instance().GetRegisterConnection().IsConnected())
    {
        shared::StaticSharedContext::Stop();
        return;
    }
    // Create component
    shared::CreateEntity();
    shared::CreateComponent(SomeTestType::_dsc, 2);

    SomeTestType foundValue;
    bool foundSomeType = shared::GetValue<SomeTestType>(2, &foundValue);
    EXPECT_TRUE(foundSomeType);

    

    shared::StaticSharedContext::Stop();
}
