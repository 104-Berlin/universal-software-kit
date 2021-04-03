#include "engine.h"
#include "prefix_editor.h"

namespace Editor {

	EOpenGLShader::EOpenGLShader(const EString& vertexSource, const EString& fragmentSource)
		: fUniformBufferSize(0)
	{
		fShaderSource = "#shader vertex\n" + vertexSource + "#shader fragment\n" + fragmentSource;
		CompileShader();
	}

	EOpenGLShader::~EOpenGLShader()
	{
		delete fUniformBuffer;
		IN_RENDER1(fRendererID, {
				glCall(glDeleteProgram(fRendererID));
			})
	}

	void EOpenGLShader::CompileShader()
	{
		EString* shaderSources = new EString[2];

		const char* typeToken = "#shader";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = fShaderSource.find(typeToken, 0);
		while (pos != EString::npos)
		{
			size_t eol = fShaderSource.find_first_of("\r\n", pos);
			//TODO: Add ASSERT
			//IN_CORE_ASSERT(eol != EString::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			EString type = fShaderSource.substr(begin, eol - begin);
			//TODO: AddAssert
			//IN_CORE_ASSERT(type == "vertex" || type == "fragment", "Invalid shader type specified");

			size_t nextLinePos = fShaderSource.find_first_not_of("\r\n", eol);
			pos = fShaderSource.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromEString(type)] = fShaderSource.substr(nextLinePos, pos - (nextLinePos == EString::npos ? fShaderSource.size() - 1 : nextLinePos));
		}



		// Send the vertex shader source code to GL
		// Note that std::EString's .c_str is NULL character terminated.
		IN_RENDER_S1(shaderSources, {
		const GLchar * source = (const GLchar*)shaderSources[0].c_str();
		GLuint vertexShader = glCall(glCreateShader(GL_VERTEX_SHADER));

		glCall(glShaderSource(vertexShader, 1, &source, 0));

		// Compile the vertex shader
		glCall(glCompileShader(vertexShader));

		GLint isCompiled = 0;
		glCall(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled));
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glCall(glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength));

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glCall(glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]));

			// We don't need the shader anymore.
			glCall(glDeleteShader(vertexShader));

			// Use the infoLog as you see fit.
			std::cout << &infoLog[0] << std::endl;

			// In this simple fRendererID, we'll just leave
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCall(glCreateShader(GL_FRAGMENT_SHADER));

		// Send the fragment shader source code to GL
		// Note that std::EString's .c_str is NULL character terminated.
		source = (const GLchar *)shaderSources[1].c_str();
		glCall(glShaderSource(fragmentShader, 1, &source, 0));

		// Compile the fragment shader
		glCall(glCompileShader(fragmentShader));

		glCall(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled));
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glCall(glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength));

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glCall(glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]));

			// We don't need the shader anymore.
			glCall(glDeleteShader(fragmentShader));
			// Either of them. Don't leak shaders.
			glCall(glDeleteShader(vertexShader));

			// Use the infoLog as you see fit.
			std::cout << &infoLog[0] << std::endl;

			// In this simple fRendererID, we'll just leave
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a fRendererID.
		// Get a fRendererID object.
		self->fRendererID = glCall(glCreateProgram());

		// Attach our shaders to our fRendererID
		glCall(glAttachShader(self->fRendererID, vertexShader));
		glCall(glAttachShader(self->fRendererID, fragmentShader));

		// Link our fRendererID
		glCall(glLinkProgram(self->fRendererID));

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glCall(glGetProgramiv(self->fRendererID, GL_LINK_STATUS, (int *)&isLinked));
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glCall(glGetProgramiv(self->fRendererID, GL_INFO_LOG_LENGTH, &maxLength));

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glCall(glGetProgramInfoLog(self->fRendererID, maxLength, &maxLength, &infoLog[0]));

			// We don't need the fRendererID anymore.
			glCall(glDeleteProgram(self->fRendererID));
			// Don't leak shaders either.
			glCall(glDeleteShader(vertexShader));
			glCall(glDeleteShader(fragmentShader));

			// Use the infoLog as you see fit.
			std::cout <<&infoLog[0] << std::endl;

			// In this simple fRendererID, we'll just leave
			return;
		}

		// Always detach shaders after a successful link.
		glCall(glDetachShader(self->fRendererID, vertexShader));
		glCall(glDetachShader(self->fRendererID, fragmentShader));
			})

		ProcessResources();
	}


	void EOpenGLShader::LoadShaderFromFile(const EString& path)
	{
		std::ifstream in(path, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			fShaderSource.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&fShaderSource[0], fShaderSource.size());
			in.close();
		}
		else
			std::cout << "Could not read sahder file " << path << std::endl;
	}

	void EOpenGLShader::ProcessResources()
	{
		const char* token;
		const char* source = fShaderSource.c_str();

		while ((token = FindToken(source, "struct")))
			ParseUniformStruct(GetBlock(token, &source));

		source = fShaderSource.c_str();
		while ((token = FindToken(source, "uniform")))
			ParseUniform(GetStatement(token, &source));

		fUniformBuffer = (byte*) malloc(fUniformBufferSize);
		memset(fUniformBuffer, 0, fUniformBufferSize);
	}

	byte* EOpenGLShader::GetUniformBuffer(const EString& name) const
	{
		byte* result = fUniformBuffer;
		for (const EShaderUniform& uni : fUniforms)
		{
			if (uni.Name == name)
				return result;
			else if (uni.Name.find(".") != EString::npos)
			{
				EString ss;
				for (u32 i = 0; i < uni.Name.size(); i++)
				{
					if (uni.Name[i] == '.') break;
					ss += uni.Name[i];
				}
				if (ss == name)
					return result;
			}
		
			result += ShaderDataTypeSize(uni.Type);
		}
		std::cout << "Could not find Uniform " << name << " in uniformBuffer" << std::endl;
		return nullptr;
	}

	void EOpenGLShader::SetUniformBuffer(const EString& name, byte* value, size_t size)
	{
		byte* ptr = GetUniformBuffer(name);
		IN_RENDER3(ptr, value, size, {
				memcpy(ptr, value, size);
			});
	}

	void EOpenGLShader::ParseUniform(const EString& statement)
	{
		std::vector<EString> tokens = Tokenize(statement);
		unsigned int index = 0;
		index++; // "uniform"
		EString typeEString = tokens[index++];
		EString name = tokens[index++];

		// Strip ; from name if present
		if (const char* s = strstr(name.c_str(), ";"))
			name = EString(name.c_str(), s - name.c_str());

		short count = 1;
		const char* namestr = name.c_str();
		if (const char* s = strstr(namestr, "["))
		{
			name = EString(namestr, s - namestr);

			const char* end = strstr(namestr, "]");
			EString c(s + 1, end - s);
			count = atoi(c.c_str());
		}


		EShaderDataType type = ShaderDataTypeFromString(typeEString);
		u32 size = ShaderDataTypeSize(type);

		if (type == EShaderDataType::Texture2D || type == EShaderDataType::TextureCube)
			fResources.push_back(name);


		if (type == EShaderDataType::None)
		{
			EShaderStruct str;
			//Find struct
			if (FindStruct(typeEString, &str))
			{
				for (EStructMember& mem : str.Members)
				{
					size = ShaderDataTypeSize(mem.Type);
					EString uniformName = name + "." + mem.Name;
					fUniforms.push_back({ uniformName, mem.Type });
					fUniformBufferSize += count * size;
				}
			}
		}
		else
		{
			fUniforms.push_back({ name, type });
			fUniformBufferSize += count * size;
		}
	}

	bool EOpenGLShader::FindStruct(const EString& name, EShaderStruct* str)
	{
		if (!str)
		{
			std::cout << "To find a struct give a valid struct to store data in!" << std::endl;
			return false;
		}

		for (EShaderStruct& s : fStructs)
			if (s.Name == name)
				*str = s;
		return (bool)str;
	}

	void EOpenGLShader::ParseUniformStruct(const EString& block)
	{
		std::vector<EString> tokens = Tokenize(block);
		std::vector<EStructMember> members;
		u32 structSize = 0;
		unsigned int index = 0;
		index++; // "struct"
		EString name = tokens[index++];
		index++; //;
		while (index < tokens.size())
		{
			if (tokens[index] == "}")
				break;
			EString typeEString = tokens[index++];
			EString memberName = tokens[index++];

			// Strip ; from name if present
			if (const char* s = strstr(memberName.c_str(), ";"))
				memberName = EString(memberName.c_str(), s - memberName.c_str());

			short count = 1;
			const char* namestr = memberName.c_str();
			if (const char* s = strstr(namestr, "["))
			{
				memberName = EString(namestr, s - namestr);

				const char* end = strstr(namestr, "]");
				EString c(s + 1, end - s);
				count = atoi(c.c_str());
			}
			EShaderDataType type = ShaderDataTypeFromString(typeEString);
			u32 size = ShaderDataTypeSize(type);

			structSize += count * size;
			members.push_back({type, memberName});
		}
		fStructs.push_back({ name, members, structSize });
	}

	void EOpenGLShader::Bind() const
	{
		IN_RENDER_S({
				glCall(glUseProgram(self->fRendererID));
			})
	}

	void EOpenGLShader::Unbind() const
	{
#ifdef IN_DEBUG
		IN_RENDER({
				glCall(glUseProgram(0));
			})
#endif
	}

	u32 EOpenGLShader::ShaderTypeFromEString(const EString& type)
	{
		if (type == "vertex")
			return 0;
		else if (type == "fragment")
			return 1;
		std::cout << "Shader type unknown!" << std::endl;
		return -1;
	}

	int EOpenGLShader::GetUniformLocation(const EString& name)
	{
		if (fUniformCache.find(name) != fUniformCache.end())
			return fUniformCache[name];
		
		int result = glGetUniformLocation(fRendererID, name.c_str());
		if (result == -1)
			std::cout << "Could not find Uniform " << name << std::endl;
		else
			fUniformCache[name] = result;
		return result;
	}

	void EOpenGLShader::UploadUniformBuffer()
	{
		IN_RENDER_S({
			byte* buffer = self->fUniformBuffer;
			for (EShaderUniform& uniform : self->fUniforms)
			{
				switch (uniform.Type)
				{
				case EShaderDataType::Matrix3: self->SetUniformMat3(uniform.Name, *((const glm::mat3*)buffer)); break;
				case EShaderDataType::Matrix4: self->SetUniformMat4(uniform.Name, *((const glm::mat4*)buffer)); break;
				case EShaderDataType::Float: self->SetUniform1f(uniform.Name, *((const float*)buffer)); break;
				case EShaderDataType::Float2: self->SetUniform2f(uniform.Name, *((const EVec2*)buffer)); break;
				case EShaderDataType::Float3: self->SetUniform3f(uniform.Name, *((const EVec3*)buffer)); break;
				case EShaderDataType::Float4: self->SetUniform4f(uniform.Name, *((const EVec4*)buffer)); break;
				case EShaderDataType::Int: self->SetUniform1i(uniform.Name, *((const int*)buffer)); break;
				case EShaderDataType::Bool: self->SetUniform1i(uniform.Name, *((const bool*)buffer)); break;
				case EShaderDataType::Texture2D:
				case EShaderDataType::TextureCube: self->SetUniform1i(uniform.Name, (int&)*((int*)buffer)); break;
				default:
					std::cout << "Cant decide what uniform type to upload" << std::endl;
					break;
				}
				buffer += ShaderDataTypeSize(uniform.Type);
			}
		})
	}

	void EOpenGLShader::SetUniform1i(const EString& name, const int& value)
	{
		IN_RENDER_S2(name, value, {
			glCall(glUniform1i(self->GetUniformLocation(name), value));
		})
	}

	void EOpenGLShader::SetUniform1f(const EString& name, const float& value)
	{
		IN_RENDER_S2(name, value, {
			glCall(glUniform1f(self->GetUniformLocation(name), value));
		})
	}

	void EOpenGLShader::SetUniform2f(const EString& name, const EVec2& value)
	{
		IN_RENDER_S2(name, value, {
			glCall(glUniform2f(self->GetUniformLocation(name), value.x, value.y));
		})
	}

	void EOpenGLShader::SetUniform3f(const EString& name, const EVec3& value)
	{
		IN_RENDER_S2(name, value, {
			glCall(glUniform3f(self->GetUniformLocation(name), value.x, value.y, value.z));
		})
	}

	void EOpenGLShader::SetUniform4f(const EString& name, const EVec4& value)
	{
		IN_RENDER_S2(name, value, {
					glCall(glUniform4f(self->GetUniformLocation(name), value.x, value.y, value.z, value.w));
		})
	}

	void EOpenGLShader::SetUniformMat3(const EString& name, const glm::mat3& value)
	{
		IN_RENDER_S2(name, value, {
			glCall(glUniformMatrix3fv(self->GetUniformLocation(name), 1, GL_FALSE, &value[0][0]));
		})
	}

	void EOpenGLShader::SetUniformMat4(const EString& name, const glm::mat4& value)
	{
		IN_RENDER_S2(name, value, {
				glCall(glUniformMatrix4fv(self->GetUniformLocation(name), 1, GL_FALSE, &value[0][0]));
			})
	}

	int EOpenGLShader::GetResourceSlot(const EString& name) const
	{
		auto it = std::find(fResources.begin(), fResources.end(), name);
		if (it == fResources.end())
			return -1;
		return std::distance(fResources.begin(), it);
	}






















	EVector<EString> SplitString(const EString& str, const EString& delimiters)
	{

		size_t start = 0;
		size_t end = str.find_first_of(delimiters);

		EVector<EString> result;

		while (end <= EString::npos)
		{
			EString token = str.substr(start, end - start);
			if (!token.empty())
				result.push_back(token);

			if (end == EString::npos)
				break;

			start = end + 1;
			end = str.find_first_of(delimiters, start);
		}

		return result;
	}

	EVector<EString> SplitString(const EString& str, const char delimiter)
	{
		return SplitString(str, EString(1, delimiter));
	}

	EVector<EString> Tokenize(const EString& str)
	{
		EVector<std::string> tokens;
		std::istringstream iss(str);
		for (EString s; iss >> s; )
			tokens.push_back(s);

		return tokens;
		//return SplitString(str, " \t\n");
	}

	EVector<EString> GetLines(const EString& str)
	{
		return SplitString(str, "\n");
	}

	const char* FindToken(const char* str, const EString& token)
	{
		const char* t = str;
		while ((t = strstr(t, token.c_str())))
		{
			bool left = str == t || isspace(t[-1]);
			bool right = !t[token.size()] || isspace(t[token.size()]);
			if (left && right)
				return t;

			t += token.size();
		}
		return nullptr;
	}

	const char* FindToken(const EString& str, const EString& token)
	{
		return FindToken(str.c_str(), token);
	}


	EString GetStatement(const char* str, const char** outPosition)
	{
		const char* end = strstr(str, ";");
		if (!end)
			return EString(str);

		if (outPosition)
			*outPosition = end;
		u32 length = end - str + 1;
		return EString(str, length);
	}

	EString GetBlock(const char* str, const char** outPosition)
	{
		const char* end = strstr(str, "}");
		if (!end)
			return EString(str);

		if (outPosition)
			*outPosition = end;
		u32 length = end - str + 1;
		return EString(str, length);
	}

	EString GetBlock(const EString& string, u32 offset)
	{
		const char* str = string.c_str() + offset;
		return GetBlock(str);
	}

}
