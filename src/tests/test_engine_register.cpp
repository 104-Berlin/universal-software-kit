#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_shared.h>

using namespace Engine;
using namespace entt::literals;
struct MyTestComponent
{
	int SomeInteger;
};

TEST(RegisterTest, Basics)
{
	EScene scene;
	EObject object1 = EObject::Create(&scene);

	MyTestComponent& comp = object1.AddComponent<MyTestComponent>();
	comp.SomeInteger = 32;

	EXPECT_TRUE(object1.HasComponent<MyTestComponent>());
	EXPECT_EQ(object1.GetComponent<MyTestComponent>().SomeInteger, 32);
}

TEST(RegisterTest, EventSystem)
{
	EScene scene;
	EObject object1 = EObject::Create(&scene);

	object1.AddChangeEvent<MyTestComponent>([](EObjectChangeEvent<MyTestComponent> changeEvent){
		MyTestComponent newValue = changeEvent.NewValue;
		E_INFO("NewValue: " + std::to_string(newValue.SomeInteger));
	});

	MyTestComponent newValues{};
	newValues.SomeInteger = 10;

	object1.UpdateComponent<MyTestComponent>(newValues);

}