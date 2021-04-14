#pragma once

namespace Editor {

	

	class EOpenGLShader : public EShader
	{
	public:
		EOpenGLShader(const EString& path);
		EOpenGLShader(const EString& vertexSource, const EString& fragmentSource);
		virtual ~EOpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void UploadUniformBuffer() override;

		virtual void SetUniform1i(const EString& name, const int& value) override;
		virtual void SetUniform1f(const EString& name, const float& value) override;
		virtual void SetUniform2f(const EString& name, const EVec2& value) override;
		virtual void SetUniform3f(const EString& name, const EVec3& value) override;
		virtual void SetUniform4f(const EString& name, const EVec4& value) override;
		virtual void SetUniformMat4(const EString& name, const glm::mat4& value) override;
		void SetUniformMat3(const EString& name, const glm::mat3& value);

		virtual byte* GetUniformBuffer(const EString& name) const override;
		virtual void SetUniformBuffer(const EString& name, byte* data, size_t size) override;

		virtual int GetResourceSlot(const EString& name) const override;
	private:
		int GetUniformLocation(const EString& name);
		void LoadShaderFromFile(const EString& path);
		void ProcessResources();
		void ParseUniform(const EString& statement);
		void ParseUniformStruct(const EString& statement);
		u32 ShaderTypeFromEString(const EString& type);
		void CompileShader();
		bool FindStruct(const EString& name, EShaderStruct* struc);
	private:
		std::unordered_map<EString, int> fUniformCache;
		byte* fUniformBuffer;
		u32 fUniformBufferSize;
		EVector<EShaderUniform> fUniforms;
		EVector<EShaderStruct> fStructs;
		EVector<EString> fResources;
		EString fShaderSource;
		u32 fRendererID;
	};

	////// PUT THIS SOMEWHERE ELSE

	EVector<EString> SplitString(const EString& str, const EString& delimiters);
	EVector<EString> SplitString(const EString& str, const char delimiter);
	EVector<EString> Tokenize(const EString& str);
	EVector<EString> GetLines(const EString& str);

	const char* FindToken(const char* str, const EString& token);
	const char* FindToken(const EString& str, const EString& token);
	EString GetStatement(const char* str, const char** outPosition = nullptr);

	EString GetBlock(const char* str, const char** outPosition = nullptr);
	EString GetBlock(const EString& string, u32 offset = 0);

}
