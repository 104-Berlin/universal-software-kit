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

	entt::meta<MyTestComponent>().type("MyTestComponent"_hs);
	entt::meta<MyTestComponent>().data<&MyTestComponent::SomeInteger>("SomeInteger"_hs).prop("Name"_hs, "SomeInteger");



	for (auto type : entt::resolve<MyTestComponent>().data())
	{
		E_INFO((char*) type.prop("Name"_hs).value().data());
	}
}