#pragma once

namespace Editor {

    class EUIRenderer
    {
    private:
        ERef<EVertexArray>  fVertexArray;
        ERef<EVertexBuffer> fVertexBuffer;
        ERef<EIndexBuffer>  fIndexBuffer;
        ERef<EShader>       fShader;
        ERef<ETexture2D>    fFontTexture;

        bool                fIsInitialized;
        GLFWwindow*         fMainWindow;
        ImGuiContext*       fImGuiContext;
    public:
        EUIRenderer();
        void Init(GLFWwindow* window);

        void Begin();
        void Render();
        void DrawData(ImDrawData* drawData);
        void End();

        bool IsInitialized() const { return fIsInitialized; }

        ImGuiContext* GetImGuiContext() const { return fImGuiContext; }
        void ResetImGuiContext();
    private:
        void ResetRenderState(ImDrawData* drawData, int fbWidth, int fbHeight);
        void CreateRenderingStorage();
        void CreateFontAtlas();
        void RenderWindow(ImGuiViewport* viewport, void* data);
    };

}