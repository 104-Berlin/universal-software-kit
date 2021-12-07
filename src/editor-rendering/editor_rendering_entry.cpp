#include "editor_rendering.h"

#include <iostream>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

/*#include "Common/interface/RefCntAutoPtr.hpp"

#include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"*/

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

/*using namespace Diligent;

RefCntAutoPtr<IRenderDevice>  pDevice;
RefCntAutoPtr<IDeviceContext> pImmediateContext;
RefCntAutoPtr<ISwapChain>     pSwapChain;*/

void editor_rendering::RunApplicationLoop(std::function<void()> initFunction, std::function<void()> RenderImGui, std::function<void()> CleanUp, void(*SetImGuiContext)(ImGuiContext*))
{
    if (glfwInit() != GLFW_TRUE)
        return;


#ifdef EWIN
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
#ifdef EMAC
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

    GLFWwindow* wnd = glfwCreateWindow(1270, 720, "Test Diligent", NULL, NULL);
    if (!wnd) { std::cout << "Could not init wind" << std::endl; return; }
    glfwSetWindowSizeLimits(wnd, 320, 240, GLFW_DONT_CARE, GLFW_DONT_CARE);
/*
#ifdef EWIN
    Win32NativeWindow Window{glfwGetWin32Window(wnd)};
    auto GetEngineFactoryOpenGL = LoadGraphicsEngineOpenGL();
#endif

#ifdef EMAC
    Diligent::MacOSNativeWindow Window;
    glfwMakeContextCurrent(wnd);
#endif

    auto* pFactoryOpenGL = GetEngineFactoryOpenGL();

    Diligent::EngineGLCreateInfo EngineCI;
    EngineCI.Window = Window;

    SwapChainDesc SCDesc;
    pFactoryOpenGL->CreateDeviceAndSwapChainGL(EngineCI, &pDevice, &pImmediateContext, SCDesc, &pSwapChain);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(wnd, true);
    ImGui_ImplOpenGL3_Init();




    bool running = true;
    while (running)
    {
        ITextureView* pRTV = pSwapChain->GetCurrentBackBufferRTV();
        pImmediateContext->SetRenderTargets(1, &pRTV, nullptr, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        const float ClearColor[4] = {1.0f, 0.0f, 1.0f, 1.0f};
        pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_VERIFY);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Test");
        ImGui::Text("Hello, world!");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        pImmediateContext->Flush();
        pSwapChain->Present();

        glfwSwapBuffers(wnd);
        glfwPollEvents();
        running = !glfwWindowShouldClose(wnd);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(wnd);
    glfwTerminate();
*/
    return;
}