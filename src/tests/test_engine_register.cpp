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
bool convert::setter<Vector>(EProperty* prop, const Vector& vec)
{
	if (prop->GetDescription().GetType() != EValueType::STRUCT) { return false; }
	EStructProperty* property = static_cast<EStructProperty*>(prop);

	EValueProperty<double>* xProp = (EValueProperty<double>*)property->GetProperty("X");
	EValueProperty<double>* yProp = (EValueProperty<double>*)property->GetProperty("Y");
	EValueProperty<double>* zProp = (EValueProperty<double>*)property->GetProperty("Z");
	if (xProp && yProp && zProp)
	{
		xProp->SetValue(vec.x);
		yProp->SetValue(vec.y);
		zProp->SetValue(vec.z);
		return true;
	}
	return false;
}

template <>
bool convert::getter<Vector>(const EProperty* prop, Vector* outVec)
{
	if (prop->GetDescription().GetType() != EValueType::STRUCT) { return false; }
	const EStructProperty* property = static_cast<const EStructProperty*>(prop);

	const EValueProperty<double>* xProp = (EValueProperty<double>*)property->GetProperty("X");
	const EValueProperty<double>* yProp = (EValueProperty<double>*)property->GetProperty("Y");
	const EValueProperty<double>* zProp = (EValueProperty<double>*)property->GetProperty("Z");
	if (xProp && yProp && zProp)
	{
		outVec->x = (float)xProp->GetValue();
		outVec->y = (float)yProp->GetValue();
		outVec->z = (float)zProp->GetValue();
		return true;
	}
	return false;
}




TEST(RegisterTest, Basics)
{
	EValueDescription vector = EValueDescription(EValueType::STRUCT, "Vector");
	vector
		.AddStructField("X", DoubleDescription)
		.AddStructField("Y", DoubleDescription)
		.AddStructField("Z", DoubleDescription);

	EValueDescription someEnum = EValueDescription(EValueType::ENUM, "SomeEnum");
	someEnum
		.AddEnumOption("One")
		.AddEnumOption("Two")
		.AddEnumOption("Three");

	EValueDescription vectorList = vector.GetAsArray();

	EValueDescription myTestComponent = EValueDescription(EValueType::STRUCT, "MyTestComponent");
	myTestComponent
		.AddStructField("MyString", StringDescription)
		.AddStructField("MyInteger", IntegerDescription)
		.AddStructField("MyBool", BoolDescription)
		.AddStructField("MyDouble", DoubleDescription)
		.AddStructField("Vector", vector)
		.AddStructField("Enum", someEnum)
		.AddStructField("VectorArray", vectorList);
	
	EDataBase scene;

	bool componentCreated = false;
	scene.GetEventDispatcher().Connect<EntityChangeEvent>([&componentCreated, myTestComponent](EntityChangeEvent event){
		if (event.Type.Value == EntityChangeType::COMPONENT_ADDED)
		{
			componentCreated = true;
		}
	});

	EDataBase::Entity entity = scene.CreateEntity();
	EXPECT_FALSE(componentCreated);
	scene.AddComponent(entity, myTestComponent);
	scene.UpdateEvents();
	EXPECT_TRUE(componentCreated);
	scene.AddComponent(entity, vector);

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

		EXPECT_STREQ(storage->GetDescription().GetId().c_str(), myTestComponent.GetId().c_str());

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
			enumProperty->SetCurrentValue<EString>("Two");

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

			Vector v;
			EXPECT_TRUE(vectorProperty->GetValue<Vector>(v));
			EXPECT_EQ(v.x, newVecValue.x);
			EXPECT_EQ(v.y, newVecValue.y);
			EXPECT_EQ(v.z, newVecValue.z);

			EXPECT_STREQ(stringValue->GetValue().c_str(), "Hello World");
			EXPECT_TRUE(boolValue->GetValue());

			EXPECT_EQ(enumProperty->GetCurrentValue(), 1);

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

/**
 * Modern Register Components with C Represenation
 */

E_STORAGE_STRUCT(Vec,
		(double, X),
		(double, Y),
		(double, Z)
	);

	E_STORAGE_ENUM(MyEnum,
		ONE, TOW
	);

	E_STORAGE_STRUCT(MyTestComponent,
		(EString, MyString),
		(double, MyDouble),
		(bool, MyBool),
		(Vec, Vector),
		(MyEnum, Enum),
		(EAny, MyAny),
		(EVector<Vec>, VectorArray)
	);

TEST(EDataBase, RegisterComponentWithCRepresenation)
{
	MyTestComponent myTestComponent;

	EDataBase dataBase;
	EDataBase::Entity entity1 = dataBase.CreateEntity();
	dataBase.AddComponent<MyTestComponent>(entity1);
	EStructProperty* storage = dataBase.GetComponent(entity1, MyTestComponent::_dsc);
	storage->SetValue<MyTestComponent>(myTestComponent);

	ERef<EStructProperty> someAnyValue = ERef<EStructProperty>(static_cast<EStructProperty*>(EProperty::CreateFromDescription(Vec::_dsc.GetId(), Vec::_dsc)));
	someAnyValue->SetValue<Vec>(Vec{1, 2, 3});


	if (storage->GetValue<MyTestComponent>(myTestComponent))
	{
		myTestComponent.MyAny.SetValue(someAnyValue);
		myTestComponent.MyDouble = 10.4;
		myTestComponent.MyString = "Hello World";

		storage->SetValue<MyTestComponent>(myTestComponent);
	}

	MyTestComponent myTestComponent2;
	if (storage->GetValue<MyTestComponent>(myTestComponent2))
	{
		EXPECT_EQ(myTestComponent2.MyDouble, 10.4);
		EXPECT_STREQ(myTestComponent2.MyString.c_str(), "Hello World");
		Vec v;
		if (myTestComponent2.MyAny.Value() && convert::getter(myTestComponent.MyAny.Value(), &v))
		{
			EXPECT_EQ(v.X, 1);
			EXPECT_EQ(v.Y, 2);
			EXPECT_EQ(v.Z, 3);
		}
	}

	

	
}