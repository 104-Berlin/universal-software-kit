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

void SetCustomColorStyle()
{
    ImGuiStyle* style = &ImGui::GetStyle();
    style->WindowRounding = 0;
    style->WindowBorderSize = 0;
    style->FrameRounding = 0;
    style->FrameBorderSize = 0;
    style->FramePadding = {4.0f, 4.0f};
    style->TabBorderSize = 0;
    style->TabRounding = 0;
    style->TabMinWidthForCloseButton = FLT_MAX;
    style->WindowMenuButtonPosition = ImGuiDir_Right;

    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_Border]                 = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.335f, 0.335f, 0.335f, 1.000f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
	colors[ImGuiCol_Button]                 = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
	colors[ImGuiCol_Separator]              = ImVec4(0.000f, 0.000f, 0.000f, 0.137f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.25f, 0.25f, 0.25f, 0.9f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 0.9f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.43f, 0.43f, 0.43f, 0.9f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.11f, 0.11f, 0.11f, 0.6f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.34f, 0.34f, 0.34f, 0.6f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(0.45f, 0.45f, 0.45f, 0.6f);        // Preview overlay color when about to docking something
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.15f, 0.15f, 0.15f, 0.7f);
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
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    SetCustomColorStyle();

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