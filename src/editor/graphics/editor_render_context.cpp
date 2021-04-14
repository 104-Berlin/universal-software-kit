#include "engine.h"
#include "prefix_editor.h"

namespace Editor {

    ERenderingType ERenderContext::Renderer = ERenderingType::OpenGL;
    ERenderContext* ERenderContext::s_Instance = nullptr;

    void ERenderContext::Create(GLFWwindow* window)
    {
        // TODO: ASSERT
		switch (ERenderContext::Renderer)
		{
		case ERenderingType::OpenGL:    s_Instance = new EOpenGLRenderContext(window);
        case ERenderingType::None:		break;
		}
    }

    void ERenderContext::Destroy()
    {
        if (s_Instance)
            delete s_Instance;
    }

}
