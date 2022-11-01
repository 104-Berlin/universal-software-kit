#include "renderer.h"
#include "GLFW/glfw3.h"

int main()
{
    glfwInit();


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
    glfwWindowHint(GLFW_SAMPLES, 8);


    GLFWwindow* window = glfwCreateWindow(1270, 720, "USK", NULL, NULL);

    glfwMakeContextCurrent(window);
    sr::srLoad((sr::SRLoadProc)glfwGetProcAddress);

    int frameWidth = 0;
    int frameHeight = 0;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glfwGetFramebufferSize(window, &frameWidth, &frameHeight);

        sr::srClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sr::srNewFrame(frameWidth, frameHeight);

        sr::srBegin(sr::EBatchDrawMode::QUADS);
        sr::srColor3f(0.6f, 0.3f, 0.2f);
        sr::srVertex2f(0.0f,    0.0f);
        sr::srVertex2f(100.0f,  0.0f);
        sr::srVertex2f(100.0f,  100.0f);
        sr::srVertex2f(0.0f,    100.0f);
        sr::srEnd();

        sr::srEndFrame();

        glfwSwapBuffers(window);
    }
    return 0;
}