#include "engine.h"
#include "prefix_shared.h"
#include "prefix_editor.h"

#include <GLFW/glfw3.h>

using namespace Editor;

void Application::Run()
{
    if (!glfwInit())
    {
        E_ERROR("Could not init glfw!");   
        return;
    }
    GLFWwindow* mainWindow = glfwCreateWindow(1270, 720, "Editor", NULL, NULL);
    if (!mainWindow)
    {
        E_ERROR("Could not create Window!");
        return;
    }

    glfwMakeContextCurrent(mainWindow);

    while (!glfwWindowShouldClose(mainWindow))
    {
        glfwPollEvents();

        glfwSwapBuffers(mainWindow);
    }
    glfwTerminate();
}