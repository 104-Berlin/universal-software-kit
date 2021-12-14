#include "editor_rendering.h"

#include <iostream>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>


#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace Diligent;

RefCntAutoPtr<IRenderDevice>  pDevice;
RefCntAutoPtr<IDeviceContext> pImmediateContext;
RefCntAutoPtr<ISwapChain>     pSwapChain;

static ImGuiContext* imguiContext = nullptr;

ImGuiContext* editor_rendering::GetCurrentImGuiContext()
{
    return imguiContext;
}

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
    imguiContext = ImGui::GetCurrentContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(wnd, true);
    ImGui_ImplOpenGL3_Init();

    if (SetImGuiContext)
    {
        SetImGuiContext(imguiContext);
    }
    if (initFunction)
    {
        initFunction();
    }


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

        static bool p_open;

        static bool opt_fullscreen_persistant = true;
        static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
        bool opt_fullscreen = opt_fullscreen_persistant;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - 32));
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }


        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &p_open, window_flags);
        ImGui::PopStyleVar();


        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Dockspace		         
        ImGuiIO& io = ImGui::GetIO();		        
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {		
            ImGuiID dockspace_id = ImGui::GetID("MyDockspace");		
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);		
        }
        
        if (RenderImGui)
        {
            RenderImGui();
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        pImmediateContext->Flush();
        //pSwapChain->Present();

        glfwSwapBuffers(wnd);
        glfwPollEvents();
        running = !glfwWindowShouldClose(wnd);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (CleanUp)
    {
        CleanUp();
    }

    glfwDestroyWindow(wnd);
    glfwTerminate();

    return;
}