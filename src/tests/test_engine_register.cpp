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




TEST(RegisterTest, Basics)
{
	EStructDescription* vector = new EStructDescription("Vector");
	vector->AddField("X", DoubleDescription());
	vector->AddField("Y", DoubleDescription());
	vector->AddField("Z", DoubleDescription());

	EEnumDescription* someEnum = new EEnumDescription("SomeEnum");
	someEnum->AddOption("One");
	someEnum->AddOption("Two");
	someEnum->AddOption("Three");

	EArrayDescription* vectorList = new EArrayDescription(vector);

	EStructDescription* myTestComponent = new EStructDescription("MyTestComponent");
	myTestComponent->AddField("MyString", StringDescription());
	myTestComponent->AddField("MyInteger", IntegerDescription());
	myTestComponent->AddField("MyBool", BoolDescription());
	myTestComponent->AddField("MyDouble", DoubleDescription());
	myTestComponent->AddField("Vector", vector);
	myTestComponent->AddField("Enum", someEnum);
	myTestComponent->AddField("VectorArray", vectorList);
	
	EScene scene;

	EScene::Entity entity = scene.CreateEntity();
	scene.InsertComponent(entity, myTestComponent);
	scene.InsertComponent(entity, vector);

	EXPECT_TRUE(scene.IsAlive(entity));
	EXPECT_TRUE(scene.HasComponent(entity, vector));
	

	EXPECT_TRUE(scene.HasComponent(entity, vector));

	EXPECT_EQ(scene.GetAllEntities().size(), 1);
	EXPECT_EQ(scene.GetAllComponents(entity).size(), 2);



	scene.RemoveComponent(entity, vector);
	EXPECT_FALSE(scene.HasComponent(entity, vector));


	{
		// Set some things to the component
		EStructProperty* storage = scene.GetComponent(entity, myTestComponent);

		EXPECT_EQ(storage->GetDescription(), myTestComponent);

		Vector newVecValue{2, 3, 4};

		EValueProperty<EString>* stringValue = static_cast<EValueProperty<EString>*>(storage->GetProperty("MyString"));
		EValueProperty<double>* doubleValue = static_cast<EValueProperty<double>*>(storage->GetProperty("MyDouble"));
		EValueProperty<bool>* boolValue = static_cast<EValueProperty<bool>*>(storage->GetProperty("MyBool"));
		EStructProperty* vectorProperty = static_cast<EStructProperty*>(storage->GetProperty("Vector"));
		EEnumProperty* enumProperty = static_cast<EEnumProperty*>(storage->GetProperty("Enum"));
		EArrayProperty* arrayProperty = static_cast<EArrayProperty*>(storage->GetProperty("VectorArray"));
		EXPECT_EQ(storage->GetProperty("UNKNOWN"), nullptr);
		EXPECT_NE(stringValue, nullptr);
		EXPECT_NE(doubleValue, nullptr);
		EXPECT_NE(boolValue, nullptr);
		EXPECT_NE(vectorProperty, nullptr);
		EXPECT_NE(enumProperty, nullptr);
		EXPECT_NE(arrayProperty, nullptr);
		if (stringValue &&
			vectorProperty &&
			boolValue &&
			doubleValue &&
			arrayProperty)
		{
			doubleValue->SetValue(22.2);
			stringValue->SetValue("Hello World");
			vectorProperty->SetValue<Vector>(newVecValue);
			boolValue->SetValue(true);
			enumProperty->SetCurrentValue("Second");

			arrayProperty->AddElement();
			arrayProperty->AddElement();
		}
	}

	{
		EStructProperty* storage = scene.GetComponent(entity, myTestComponent);

		Vector newVecValue{2, 3, 4};

		EValueProperty<EString>* stringValue = static_cast<EValueProperty<EString>*>(storage->GetProperty("MyString"));
		EValueProperty<double>* doubleValue = static_cast<EValueProperty<double>*>(storage->GetProperty("MyDouble"));
		EValueProperty<bool>* boolValue = static_cast<EValueProperty<bool>*>(storage->GetProperty("MyBool"));
		EStructProperty* vectorProperty = static_cast<EStructProperty*>(storage->GetProperty("Vector"));
		EEnumProperty* enumProperty = static_cast<EEnumProperty*>(storage->GetProperty("Enum"));
		EArrayProperty* arrayProperty = static_cast<EArrayProperty*>(storage->GetProperty("VectorArray"));

		EXPECT_NE(stringValue, nullptr);
		EXPECT_NE(doubleValue, nullptr);
		EXPECT_NE(boolValue, nullptr);
		EXPECT_NE(vectorProperty, nullptr);
		EXPECT_NE(enumProperty, nullptr);
		EXPECT_NE(arrayProperty, nullptr);

		if (stringValue &&
			vectorProperty &&
			boolValue &&
			doubleValue &&
			arrayProperty)
		{
			vectorProperty->SetValue<Vector>(newVecValue);
			EXPECT_TRUE(vectorProperty->HasProperty("X"));
			EXPECT_FALSE(vectorProperty->HasProperty("WRONG"));

			Vector v = vectorProperty->GetValue<Vector>();
			EXPECT_EQ(v.x, newVecValue.x);
			EXPECT_EQ(v.y, newVecValue.y);
			EXPECT_EQ(v.z, newVecValue.z);

			EXPECT_STREQ(stringValue->GetValue().c_str(), "Hello World");
			EXPECT_TRUE(boolValue->GetValue());

			EXPECT_STREQ(enumProperty->GetCurrentValue().c_str(), "Second");

			EXPECT_EQ(arrayProperty->GetElements().size(), 2);
			EXPECT_NE(arrayProperty->GetElement(0), nullptr);
			EXPECT_EQ(arrayProperty->GetElement(3), nullptr);
			arrayProperty->RemoveElement(1);
			EXPECT_EQ(arrayProperty->GetElements().size(), 1);
			arrayProperty->Clear();
			EXPECT_EQ(arrayProperty->GetElements().size(), 0);
		}
	}

	scene.DestroyEntity(entity);

	EXPECT_FALSE(scene.HasComponent(entity, myTestComponent));
	EXPECT_FALSE(scene.IsAlive(entity));

	scene.CreateEntity();
	scene.CreateEntity();
	scene.CreateEntity();
	scene.CreateEntity();

	EXPECT_EQ(scene.GetAllEntities().size(), 4);
	scene.Clear();
	EXPECT_EQ(scene.GetAllEntities().size(), 0);
}