#pragma once

namespace Engine {

    namespace ExUI {

        // Own ImGui Render functions for text with copy option
        E_EDEXAPI bool RenderText(const char* text, const ImVec2& size, bool copy = false);

    }

}