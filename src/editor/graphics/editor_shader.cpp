#include "engine.h"
#include "prefix_editor.h"


namespace Editor {

	ERef<EShader> EShader::Create(const EString& vertexSource, const EString& fragmentSource)
	{
		//TODO: Add ASSERT
		//IN_CORE_ASSERT((bool)ERenderContext, "No RenderAPI selected");
		switch (ERenderContext::Renderer)
		{
		case ERenderingType::OpenGL:	return ERef<EOpenGLShader>(new EOpenGLShader(vertexSource, fragmentSource));
		case ERenderingType::None:		break;
		}

		return nullptr;
	}

}
