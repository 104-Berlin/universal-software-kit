#pragma once



static Engine::EValueDescription dsc_Vec2 = Engine::EValueDescription::CreateStruct("Vec2", {{"X", Engine::DoubleDescription}, {"Y", Engine::DoubleDescription}});
static Engine::EValueDescription dsc_Vec3 = Engine::EValueDescription::CreateStruct("Vec3", {{"X", Engine::DoubleDescription}, {"Y", Engine::DoubleDescription}, {"Z", Engine::DoubleDescription}});
static Engine::EValueDescription dsc_Vec4 = Engine::EValueDescription::CreateStruct("Vec4", {{"X", Engine::DoubleDescription}, {"Y", Engine::DoubleDescription}, {"Z", Engine::DoubleDescription}, {"W", Engine::DoubleDescription}});


template <>
bool convert<Engine::EStructProperty, EVec2>::setter(Engine::EStructProperty* property, const EVec2& vec)
{
    using namespace Engine;
	EValueProperty<double>* xProp = (EValueProperty<double>*)property->GetProperty("X");
	EValueProperty<double>* yProp = (EValueProperty<double>*)property->GetProperty("Y");
	if (xProp && yProp)
	{
		xProp->SetValue(vec.x);
		yProp->SetValue(vec.y);
		return true;
	}
	return false;
}

template <>
bool convert<Engine::EStructProperty, EVec2>::getter(const Engine::EStructProperty* property, EVec2* outVec)
{
    using namespace Engine;
	const EValueProperty<double>* xProp = (EValueProperty<double>*)property->GetProperty("X");
	const EValueProperty<double>* yProp = (EValueProperty<double>*)property->GetProperty("Y");
	if (xProp && yProp)
	{
		outVec->x = (float)xProp->GetValue();
		outVec->y = (float)yProp->GetValue();
		return true;
	}
	return false;
}



template <>
bool convert<Engine::EStructProperty, EVec3>::setter(Engine::EStructProperty* property, const EVec3& vec)
{
    using namespace Engine;
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
bool convert<Engine::EStructProperty, EVec3>::getter(const Engine::EStructProperty* property, EVec3* outVec)
{
    using namespace Engine;
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



template <>
bool convert<Engine::EStructProperty, EVec4>::setter(Engine::EStructProperty* property, const EVec4& vec)
{
    using namespace Engine;
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
bool convert<Engine::EStructProperty, EVec4>::getter(const Engine::EStructProperty* property, EVec4* outVec)
{
    using namespace Engine;
	const EValueProperty<double>* xProp = (EValueProperty<double>*)property->GetProperty("X");
	const EValueProperty<double>* yProp = (EValueProperty<double>*)property->GetProperty("Y");
	const EValueProperty<double>* zProp = (EValueProperty<double>*)property->GetProperty("Z");
	const EValueProperty<double>* wProp = (EValueProperty<double>*)property->GetProperty("W");
	if (xProp && yProp && zProp && wProp)
	{
		outVec->x = (float)xProp->GetValue();
		outVec->y = (float)yProp->GetValue();
		outVec->z = (float)zProp->GetValue();
		outVec->w = (float)wProp->GetValue();
		return true;
	}
	return false;
}