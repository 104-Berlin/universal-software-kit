#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_shared.h>

using namespace Engine;

/*struct MyEvent
{
    int SomeValue;
};*/

E_STORAGE_TYPE(MyEvent, 
    (int, SomeValue)
)

TEST(EventTests, Basics)
{
    EEventDispatcher eventDispatcher;


    i32 testingValue = 0;


    eventDispatcher.Connect<MyEvent>([&testingValue](MyEvent data){
        testingValue += data.SomeValue;
    });

    MyEvent eventData;
    eventData.SomeValue = 10;


    eventDispatcher.Post(MyEvent::_dsc, eventData);
    eventDispatcher.Post(MyEvent::_dsc, eventData);
    eventDispatcher.Update();

    // Check if event got fired with correct value
    EXPECT_EQ(testingValue, 20);

    // Should not call anything
    eventDispatcher.Update();
    EXPECT_EQ(testingValue, 20);
}