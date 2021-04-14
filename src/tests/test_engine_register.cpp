#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_shared.h>

using namespace Engine;

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