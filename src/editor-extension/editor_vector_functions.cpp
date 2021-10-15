#include "editor_extension.h"

ImVec2 ImConvert::GlmToImVec2(const EVec2& vector)
{
	return {vector.x, vector.y};
}

ImVec4 ImConvert::GlmToImVec4(const EVec4& vector)
{
	return {vector.x, vector.y, vector.z, vector.w};
}


EVec2 ImConvert::ImToGlmVec2(const ImVec2& vector)
{
	return {vector.x, vector.y};
}

EVec4 ImConvert::ImToGlmVec4(const ImVec4& vector)
{
	return {vector.x, vector.y, vector.z, vector.w};
}




template <>
bool convert::setter<EVec2>(Engine::EProperty* prop, const EVec2& vec)
{
    using namespace Engine;
	if (prop->GetDescription().GetType() != EValueType::STRUCT) { return false; }
	EStructProperty* property = static_cast<EStructProperty*>(prop);

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
bool convert::getter<EVec2>(const Engine::EProperty* prop, EVec2* outVec)
{
    using namespace Engine;
	if (prop->GetDescription().GetType() != EValueType::STRUCT) { return false; }
	const EStructProperty* property = static_cast<const EStructProperty*>(prop);

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
bool convert::setter<EVec3>(Engine::EProperty* prop, const EVec3& vec)
{
    using namespace Engine;
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
bool convert::getter<EVec3>(const Engine::EProperty* prop, EVec3* outVec)
{
    using namespace Engine;
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


template <>
bool convert::setter<EVec4>(Engine::EProperty* prop, const EVec4& vec)
{
    using namespace Engine;
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
bool convert::getter<EVec4>(const Engine::EProperty* prop, EVec4* outVec)
{
    using namespace Engine;
	if (prop->GetDescription().GetType() != EValueType::STRUCT) { return false; }
	const EStructProperty* property = static_cast<const EStructProperty*>(prop);

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


#ifndef EWIN
template <>
#endif
Engine::EValueDescription Engine::getdsc::GetDescription<EVec2>()
{
	return EVec2_dsc;
}


#ifndef EWIN
template <>
#endif
Engine::EValueDescription Engine::getdsc::GetDescription<EVec3>()
{
	return EVec3_dsc;
}

#ifndef EWIN
template <>
#endif
Engine::EValueDescription Engine::getdsc::GetDescription<EVec4>()
{
	return EVec4_dsc;
}