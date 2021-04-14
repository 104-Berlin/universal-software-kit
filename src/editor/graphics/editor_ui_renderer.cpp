#include "engine.h"
#include "prefix_editor.h"
#include "backends/imgui_impl_glfw.h"

using namespace Editor;

const char* vertex_shader =
        "#version 330 core\n"
        "layout (location = 0) in vec2 Position;\n"
        "layout (location = 1) in vec2 UV;\n"
        "layout (location = 2) in vec4 Color;\n"
        "uniform mat4 ProjMtx;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "    Frag_UV = UV;\n"
        "    Frag_Color = Color;\n"
        "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";


const char* fragment_shader =
        "#version 330 core\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "uniform sampler2D Texture;\n"
        "layout (location = 0) out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
        "    //Out_Color = vec4(Frag_UV, 0.0, 1.0);\n"
        "}\n";



// WARNING!!!
// This struct is copied from imgui_impl_glfw.cpp
// If the struct changes in the impl file the programm will crash
struct ImGuiViewportDataGlfw
{
    GLFWwindow* Window;
    bool        WindowOwned;
    int         IgnoreWindowPosEventFrame;
    int         IgnoreWindowSizeEventFrame;

    ImGuiViewportDataGlfw()  { Window = NULL; WindowOwned = false; IgnoreWindowSizeEventFrame = IgnoreWindowPosEventFrame = -1; }
};

static void Glfw_RenderWindow(ImGuiViewport* viewport, void*)
{
    ESharedBuffer data = ESharedBuffer().InitWith<ImGuiViewportDataGlfw>(viewport->PlatformUserData, sizeof(ImGuiViewportDataGlfw));
    if (ERenderContext::Renderer == ERenderingType::OpenGL)
    {
        IN_RENDER1(data, {
            glfwMakeContextCurrent(data.Data<ImGuiViewportDataGlfw>()->Window);
        })
    }
}

static void Glfw_SwapBuffers(ImGuiViewport* viewport, void*)
{
    ESharedBuffer data = ESharedBuffer().InitWith<ImGuiViewportDataGlfw>(viewport->PlatformUserData, sizeof(ImGuiViewportDataGlfw));
    if (ERenderContext::Renderer == ERenderingType::OpenGL)
    {
        IN_RENDER1(data, {
            glfwMakeContextCurrent(data.Data<ImGuiViewportDataGlfw>()->Window);
            glfwSwapBuffers(data.Data<ImGuiViewportDataGlfw>()->Window);
            glfwSwapBuffers(data.Data<ImGuiViewportDataGlfw>()->Window);
        })
    }
}

void Render_Window(ImGuiViewport* vp, void* data)
{
    // Data shoulw be the current UIRenderer
    EUIRenderer* uiRenderer = (EUIRenderer*)data;
    if (!(vp->Flags & ImGuiViewportFlags_NoRendererClear))
    {
        ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        IN_RENDER1(clear_color, {
            glCall(glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w));
            glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        })
    }
    uiRenderer->DrawData(vp->DrawData);
}


 
EUIRenderer::EUIRenderer() 
{
    fIsInitialized = false;
}

void EUIRenderer::Init(GLFWwindow* window) 
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    fImGuiContext = ImGui::CreateContext();

    EFile fontFile(EBaseFolder::APPLICATION,"OpenSans-SemiBold.ttf");


    ImGuiIO& io = ImGui::GetIO();
    if (fontFile.Exist())
    {
        io.FontDefault = io.Fonts->AddFontFromFileTTF(fontFile.GetFullPath().c_str(), 20.0f);
    }

    
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    io.BackendRendererName = "104-engine";
    //io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup glfw for imgui. using the impl from imgui, because it has everything i want
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();

    platform_io.Platform_RenderWindow = Glfw_RenderWindow;
    platform_io.Platform_SwapBuffers = Glfw_SwapBuffers;
    platform_io.Renderer_RenderWindow = &Render_Window;

    CreateRenderingStorage();
}

void EUIRenderer::Begin() 
{
    CreateFontAtlas();
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
        ImGui::SetNextWindowSize(viewport->Size);
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
}

void EUIRenderer::Render() 
{
    ImGui::End();
    ImGui::Render();

    DrawData(ImGui::GetDrawData());


    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault(NULL,this);
    }
}

void EUIRenderer::DrawData(ImDrawData* drawData) 
{
    int fb_width = (int)(drawData->DisplaySize.x * drawData->FramebufferScale.x);
    int fb_height = (int)(drawData->DisplaySize.y * drawData->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

    this->ResetRenderState(drawData, fb_width, fb_height);
    


    if (fb_width <= 0 || fb_height <= 0)
        return;

    ImVec2 clip_off = drawData->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = drawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    fVertexArray->Bind();

    for (int n = 0; n < drawData->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = drawData->CmdLists[n];

        fVertexBuffer->SetData(ESharedBuffer().InitWith<ImDrawVert>((void*) cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.size_in_bytes()));
        fIndexBuffer->SetData(ESharedBuffer().InitWith<ImWchar>((void*) cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.size_in_bytes()));
        

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != NULL)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    {
                        this->ResetRenderState(drawData, fb_width, fb_height);
                    }
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec4 clip_rect;
                clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
                clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
                clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
                clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

                if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
                {
                    ImTextureID textureId = pcmd->TextureId;
                    IN_RENDER3(clip_rect, fb_height, textureId, {
                        // Apply scissor/clipping rectangle
                        glCall(glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y)));
                        // Bind texture, Draw
                        
                        glCall(glActiveTexture(GL_TEXTURE0));
                        glCall(glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)textureId));
                    })

                    u32 indexOffset = pcmd->IdxOffset;
                    u32 elementCount = pcmd->ElemCount;

                    IN_RENDER2(indexOffset, elementCount, {
                        glDrawElements(GL_TRIANGLES, (GLsizei)elementCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(indexOffset * sizeof(ImDrawIdx)));
                    })
                }
            }
        }
    }
}

void EUIRenderer::End() 
{
   
}

void EUIRenderer::ResetImGuiContext() 
{
    ImGui::SetCurrentContext(fImGuiContext);
}

void EUIRenderer::ResetRenderState(ImDrawData* drawData, int fbWidth, int fbHeight) 
{
    IN_RENDER2(fbWidth, fbHeight, {
        glCall(glViewport(0, 0, (GLsizei)fbWidth, (GLsizei)fbHeight));

        glCall(glEnable(GL_BLEND));
        glCall(glBlendEquation(GL_FUNC_ADD));
        glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        glCall(glDisable(GL_CULL_FACE));
        glCall(glDisable(GL_DEPTH_TEST));
        glCall(glEnable(GL_SCISSOR_TEST));
    })


    fShader->Bind();
    float L = drawData->DisplayPos.x;
    float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
    float T = drawData->DisplayPos.y;
    float B = drawData->DisplayPos.y + drawData->DisplaySize.y;

    
    glm::mat4 mat = glm::ortho(L, R, B, T);

    fShader->SetUniformMat4("ProjMtx", mat);
    fShader->SetUniform1i("Texture", 0);
}

void EUIRenderer::CreateRenderingStorage() 
{
    fVertexArray = EVertexArray::Create();
    fVertexBuffer = EVertexBuffer::Create(EBufferUsage::STREAM_DRAW);
    fIndexBuffer = EIndexBuffer::Create(EBufferUsage::STREAM_DRAW);

    EBufferLayout bl = {
        EBufferElement(EShaderDataType::Float2, "POSITION"),
        EBufferElement(EShaderDataType::Float2, "UV"),
        EBufferElement(EShaderDataType::Byte4, "COLOR", true)
    };
    fVertexBuffer->SetLayout(bl);

    fVertexArray->AddVertexBuffer(fVertexBuffer);
    fVertexArray->SetIndexBuffer(fIndexBuffer);

    fShader = EShader::Create(vertex_shader, fragment_shader);

    CreateFontAtlas();
}

void EUIRenderer::CreateFontAtlas() 
{
    ImGuiIO& io = ImGui::GetIO();


    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    if (!fFontTexture)
    {
        fFontTexture = ETexture2D::Create("ImGuiFontAtlas", ETextureFormat::RGBA, width, height);
    }
    fFontTexture->SetTextureData(pixels, width, height);
    IN_RENDER_S({
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->TexID = (ImTextureID)(intptr_t)self->fFontTexture->GetRendererID();
        self->fIsInitialized = true;
    });

}

void EUIRenderer::RenderWindow(ImGuiViewport* viewport, void* data) 
{
    
}
