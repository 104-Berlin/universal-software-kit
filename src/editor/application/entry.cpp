#include "sogui.h"
#include "GLFW/glfw3.h"

#include <functional>

using namespace SGui;

void graphFx(std::function<double(double)> func, double scale = 1.0);

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

    View view({100, 100});
    sr::Font font = sr::srLoadFont("G:\\repos\\universal-software-kit\\deps\\software-gui\\core\\software-rendering\\Roboto.ttf");

    int frameWidth = 0;
    int frameHeight = 0;

    double x = 0.0f;
    double lastTime = glfwGetTime();
    double deltaTime = 0.0f;

    double ups = 4;
    std::string xString = "";
    

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
        
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        sr::srClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sr::srNewFrame(frameWidth, frameHeight);

        //view.Draw();

        x += deltaTime * ups * 2.0 * sr::RAD2DEG;
        x = (int)x % (360 * 2);
        xString = "Current X: " + std::to_string(x);
        sr::srBeginPath(sr::PathType_Stroke);
        sr::srPathSetStrokeWidth(16.0f);
        sr::srPathArc({0, 0}, (x / 2.0f), x, 100.0f, 32);
        sr::srEndPath();

        sr::srDrawText(font, xString.c_str(), {-frameWidth / 2.0f, -frameHeight / 2.0f + 24.0f});

        graphFx([](double x) -> double{
            return x * x;
        }, 0.05f);



        sr::srEndFrame();
        

        glfwSwapBuffers(window);
    }
    return 0;
}

void graphFx(std::function<double(double)> func, double scale)
{
    static bool first = true;
    sr::srBegin(sr::EBatchDrawMode::LINES);
    //sr::srVertex2f(0.0f, 0.0f);
    //sr::srVertex2f(100.0f, 0.0f);
    //sr::srVertex2f(100.0f, 0.0f);
    //sr::srVertex2f(100.0f, 100.0f);
    //sr::srVertex2f(100.0f, 200.0f);
    float lastY = 0.0;
    int increase = 20;
    for (int i = 0; i < 500; i += increase)
    {   
        if (i > increase)
        {
            sr::srVertex2f((float)(i - increase), lastY);
        }
        double x = (((double)i)) * scale;
        lastY = (float) func((float)x);
        if (first)
        {
            printf("f(%f) = %f\n", x, lastY);
        }
        sr::srVertex2f(((float)i), lastY);
    }  
    sr::srEnd();
    first = false;
}