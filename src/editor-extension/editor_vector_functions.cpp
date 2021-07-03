#include "editor_extension.h"

template <>
bool convert::setter<EVec2>(Engine::EStructProperty* property, const EVec2& vec)
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
bool convert::getter<EVec2>(const Engine::EStructProperty* property, EVec2* outVec)
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
bool convert::setter<EVec3>(Engine::EStructProperty* property, const EVec3& vec)
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
bool convert::getter<EVec3>(const Engine::EStructProperty* property, EVec3* outVec)
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
bool convert::setter<EVec4>(Engine::EStructProperty* property, const EVec4& vec)
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
bool convert::getter<EVec4>(const Engine::EStructProperty* property, EVec4* outVec)
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