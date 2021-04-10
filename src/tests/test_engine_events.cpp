#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_shared.h>

using namespace Engine;

/*
EStructureDescription eventData("MyTestEvent", EDataType::STRUCTURE, {
    EStructureDescription("EventValue", EDataType::INTEGER)
});

TEST(EventTests, Basics)
{
    EEventDispatcher eventDispatcher;

    i32 testingValue = 0;


    eventDispatcher.Connect(eventData, [&testingValue](ERef<EDataHandle> data){
        if (data->GetDataType() == EDataType::STRUCTURE)
        {
            ERef<EStructureDataHandle> eventStructure = std::dynamic_pointer_cast<EStructureDataHandle>(data);
            if (eventStructure->HasFieldAt("EventValue"))
            {
                ERef<EIntegerDataHandle> addValue = eventStructure->GetFieldAt<EIntegerDataHandle>("EventValue");
                testingValue += addValue->GetValue();
            }
        }
    });

    ERef<EStructureDataHandle> myEventData = EMakeRef<EStructureDataHandle>("SomeEventData",eventData);
    ERef<EIntegerDataHandle> eventValue = myEventData->GetFieldAt<EIntegerDataHandle>("EventValue");
    eventValue->SetValue(10);


    eventDispatcher.Enqueue(myEventData);
    eventDispatcher.Update();

    // Check if event got fired with correct value
    EXPECT_EQ(testingValue, 10);

    // Should not call anything
    eventDispatcher.Update();
    EXPECT_EQ(testingValue, 10);
}*/