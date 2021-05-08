#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_shared.h>

using namespace Engine;
using namespace entt::literals;

static EComponentDescription myTestComponent("TestComponent", {
								{EValueType::INTEGER, "MyInteger"},
								{EValueType::STRING, "MyString"}});

bool TestStorage(EScene* scene, EScene::Entity entity, int valueToTest)
{
	EComponentStorage storage = scene->GetComponent(entity, myTestComponent.ID);
	if (!storage) { return false; }
	EValueProperty<i32>* someInt;
	if (!storage.GetProperty("MyInteger", &someInt))
	{
		return false;
	}
	EValueProperty<EString>* someString;
	if (!storage.GetProperty("MyString", &someString))
	{
		return false;
	}
	
	if (someInt->GetValue() != valueToTest)
	{
		return false;
	}
	

	someInt->SetValue(someInt->GetValue() + 20);
	return true;
}

TEST(RegisterTest, Basics)
{
	EScene scene;
	scene.RegisterComponent(myTestComponent);

	EScene::Entity entity = scene.CreateEntity();
	scene.InsertComponent(entity, myTestComponent.ID);

	EXPECT_TRUE(scene.IsAlive(entity));
	EXPECT_TRUE(scene.HasComponent(entity, myTestComponent.ID));

	EXPECT_TRUE(TestStorage(&scene, entity, 0));
	EXPECT_TRUE(TestStorage(&scene, entity, 20));
	EXPECT_TRUE(TestStorage(&scene, entity, 40));
	EXPECT_FALSE(TestStorage(&scene, entity, 40));


	scene.DestroyEntity(entity);

	EXPECT_FALSE(scene.HasComponent(entity, myTestComponent.ID));
	EXPECT_FALSE(scene.IsAlive(entity));
}