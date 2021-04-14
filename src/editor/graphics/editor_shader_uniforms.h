#pragma once

namespace Editor {

	enum class EShaderDataType;

	template <typename T1, typename T2, typename T3>
	struct UniformMap
	{
		T1 first;
		T2 second;
		T3 third;
		UniformMap(T1 f, T2 s, T3 t)
			: first(f), second(s), third(t)
		{}
	};

	struct EShaderUniform
	{
		EString Name;
		EShaderDataType Type;
		EShaderUniform(const EString& name, EShaderDataType type) : Name(name), Type(type) {  }
	};

	struct EStructMember
	{
		EShaderDataType Type;
		EString Name;
	};

	struct EShaderStruct
	{
		EString Name;
		EVector<EStructMember> Members;
		u32 Size;
	};

}