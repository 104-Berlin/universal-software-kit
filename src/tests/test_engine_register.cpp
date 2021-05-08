#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_shared.h>

using namespace Engine;
using namespace entt::literals;

static EComponentDescription myTestComponent("TestComponent", {{EComponentType::INTEGER, "MyInteger"}});


TEST(RegisterTest, Basics)
{
	EScene scene;
	scene.RegisterComponent(myTestComponent);

	
}