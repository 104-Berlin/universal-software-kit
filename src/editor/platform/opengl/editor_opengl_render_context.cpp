#include "engine.h"
#include "prefix_editor.h"

namespace Editor {

    EOpenGLRenderContext::EOpenGLRenderContext(GLFWwindow* window)
    {
		glfwMakeContextCurrent(window);
		int gladStatus = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        //TODO: Add ASSERT
		//IN_CORE_ASSERT(gladStatus, "Failed to initialize glad!");
        std::cout << glGetString(GL_VERSION) << std::endl;

        SetGLDefaults(window);
    }

    void EOpenGLRenderContext::SetGLDefaults(GLFWwindow* window)
    {
        GLFWwindow* win = window;

        IN_RENDER1(win, {
            glCall(glEnable(GL_DEPTH_TEST));
        })
    }

    EOpenGLRenderContext::~EOpenGLRenderContext()
    {
        
    }

    void EOpenGLRenderContext::SetClearColor(const EColor& color)
    {
        IN_RENDER_S1(color, {
            self->fClearColor = color;
        })
    }

    void EOpenGLRenderContext::Clear()
    {
        IN_RENDER_S({
            glCall(glClearColor(self->fClearColor.r, self->fClearColor.g, self->fClearColor.b, self->fClearColor.a));
            glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        })
    }

}
