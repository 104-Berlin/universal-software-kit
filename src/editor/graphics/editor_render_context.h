#pragma once

namespace Editor {

    enum class ERenderingType
    {
        None = 0,
        OpenGL = 1
    };

    class ERenderContext
    {
    protected:
    ERenderContext() = default;
    public:
    ERenderContext(const ERenderContext& other) = delete;
    virtual ~ERenderContext() = default;

    static void Create(GLFWwindow* window);
    static void Destroy();

    virtual void SetClearColor(const EColor& color) = 0;
    virtual void Clear() = 0;
    public:
    static ERenderingType Renderer;
    static ERenderContext* s_Instance;
    };

}