#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_shared.h>

using namespace Engine;

struct Vector
{
	float x;
	float y;
	float z;
};

template <>
void convert<EStructProperty, Vector>::setter(EStructProperty* property, const Vector& vec)
{
	EValueProperty<double>* xProp = (EValueProperty<double>*)property->GetProperty("X");
	EValueProperty<double>* yProp = (EValueProperty<double>*)property->GetProperty("Y");
	EValueProperty<double>* zProp = (EValueProperty<double>*)property->GetProperty("Z");
	if (xProp && yProp && zProp)
	{
		xProp->SetValue(vec.x);
		yProp->SetValue(vec.y);
		zProp->SetValue(vec.z);
	}
}

template <>
void convert<EStructProperty, Vector>::getter(const EStructProperty* property, Vector* outVec)
{
	const EValueProperty<double>* xProp = (EValueProperty<double>*)property->GetProperty("X");
	const EValueProperty<double>* yProp = (EValueProperty<double>*)property->GetProperty("Y");
	const EValueProperty<double>* zProp = (EValueProperty<double>*)property->GetProperty("Z");
	if (xProp && yProp && zProp)
	{
		outVec->x = (float)xProp->GetValue();
		outVec->y = (float)yProp->GetValue();
		outVec->z = (float)zProp->GetValue();
	}
}

static TValueTypeList vector3{
	{EValueType::DOUBLE,"X"},
	{EValueType::DOUBLE,"Y"},
	{EValueType::DOUBLE,"Z"}
};

static EComponentDescription myTestComponent("TestComponent", {
								{EValueType::INTEGER, "MyInteger"},
								{EValueType::DOUBLE, "MyDouble"},
								{EValueType::BOOL, "MyBool"},
								{EValueType::STRING, "MyString"}},
								{{"Vector", vector3}});

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

	{
		// Set some things to the component
		EComponentStorage storage = scene.GetComponent(entity, myTestComponent.ID);

		Vector newVecValue{2, 3, 4};

		EValueProperty<EString>* stringValue = nullptr;
		EValueProperty<double>* doubleValue = nullptr;
		EValueProperty<bool>* boolValue = nullptr;
		EStructProperty* vectorProperty;
		if (storage.GetProperty("MyString", &stringValue) &&
			storage.GetProperty("Vector", &vectorProperty) &&
			storage.GetProperty("MyBool", &boolValue) &&
			storage.GetProperty("MyDouble", &doubleValue))
		{
			doubleValue->SetValue(22.2);
			stringValue->SetValue("Hello World");
			vectorProperty->SetValue<Vector>(newVecValue);
			boolValue->SetValue(true);
		}
	}

	EXPECT_TRUE(TestStorage(&scene, entity, 0));
	EXPECT_TRUE(TestStorage(&scene, entity, 20));
	EXPECT_TRUE(TestStorage(&scene, entity, 40));
	EXPECT_FALSE(TestStorage(&scene, entity, 40));

	{
		EComponentStorage storage = scene.GetComponent(entity, myTestComponent.ID);

		Vector newVecValue{2, 3, 4};

		EValueProperty<EString>* stringValue = nullptr;
		EValueProperty<double>* doubleValue = nullptr;
		EValueProperty<bool>* boolValue = nullptr;
		EStructProperty* vectorProperty;
		if (storage.GetProperty("MyString", &stringValue) &&
			storage.GetProperty("Vector", &vectorProperty) &&
			storage.GetProperty("MyBool", &boolValue) &&
			storage.GetProperty("MyDouble", &doubleValue))
		{
			doubleValue->SetValue(22.2);
			stringValue->SetValue("Hello World");
			vectorProperty->SetValue<Vector>(newVecValue);
			EXPECT_TRUE(vectorProperty->HasProperty("X"));
			EXPECT_FALSE(vectorProperty->HasProperty("WRONG"));

			Vector v = vectorProperty->GetValue<Vector>();
			EXPECT_EQ(v.x, newVecValue.x);
			EXPECT_EQ(v.y, newVecValue.y);
			EXPECT_EQ(v.z, newVecValue.z);

			EXPECT_STREQ(stringValue->GetValue().c_str(), "Hello World");
			EXPECT_TRUE(boolValue->GetValue());
		}
	}

	scene.DestroyEntity(entity);

	EXPECT_FALSE(scene.HasComponent(entity, myTestComponent.ID));
	EXPECT_FALSE(scene.IsAlive(entity));
}