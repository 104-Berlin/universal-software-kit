#pragma once


#include <glfw/glfw3.h>
#include <imgui.h>

#include <functional>




namespace editor_rendering
{
    extern void RunApplicationLoop(std::function<void()> initFunction, std::function<void()> RenderImGui, std::function<void()> CleanUp, void(*SetImGuiContext)(ImGuiContext*));
}