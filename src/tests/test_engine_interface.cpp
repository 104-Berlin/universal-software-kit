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
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_TRUE(shared::StaticSharedContext::instance().GetRegisterConnection().GetConnectionStatus() == ERegisterConnection::Status::Connected);

    if (shared::StaticSharedContext::instance().GetRegisterConnection().GetConnectionStatus() != ERegisterConnection::Status::Connected)
    {
        shared::StaticSharedContext::Stop();
        return;
    }
    // Create component
    shared::CreateEntity();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EVector<EDataBase::Entity> entites = shared::GetAllEntites();
    EXPECT_EQ(entites.size(), 1);
    shared::CreateComponent(SomeTestType::_dsc, entites[0]);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    SomeTestType foundValue;
    bool foundSomeType = shared::GetValue<SomeTestType>(entites[0], &foundValue);
    EXPECT_TRUE(foundSomeType);

    

    shared::StaticSharedContext::Stop();
}
