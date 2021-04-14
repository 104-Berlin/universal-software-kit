#pragma once

static char const* gl_error_string(GLenum const err) noexcept
{
  switch (err)
  {
    // opengl 2 errors (8)
    case GL_NO_ERROR:
      return "GL_NO_ERROR";

    case GL_INVALID_ENUM:
      return "GL_INVALID_ENUM";

    case GL_INVALID_VALUE:
      return "GL_INVALID_VALUE";

    case GL_INVALID_OPERATION:
      return "GL_INVALID_OPERATION";

    case GL_STACK_OVERFLOW:
      return "GL_STACK_OVERFLOW";

    case GL_STACK_UNDERFLOW:
      return "GL_STACK_UNDERFLOW";

    case GL_OUT_OF_MEMORY:
      return "GL_OUT_OF_MEMORY";

    // opengl 3 errors (1)
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "GL_INVALID_FRAMEBUFFER_OPERATION";
    default:
      assert(!"unknown error");
      return nullptr;
  }
}


static GLenum glob_err;
#define glCall(x)                                      \
    x;                                                 \
    while ((glob_err = glGetError()) != GL_NO_ERROR)        \
    {                                                  \
        std::cout << "GL_ERROR calling \"" << #x << "\": " << gl_error_string(glob_err) << __FILE__ << std::endl; \
    }

namespace Editor
{

    class EOpenGLRenderContext : public ERenderContext
    {
    public:
        EOpenGLRenderContext(GLFWwindow* window);
        ~EOpenGLRenderContext();

        virtual void SetClearColor(const EColor &color);
        virtual void Clear();

    private:
        void SetGLDefaults(GLFWwindow* window);

    private:
        EColor fClearColor;
    };

} // namespace Engine