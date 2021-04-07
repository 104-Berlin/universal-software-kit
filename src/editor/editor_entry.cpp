#include "graphics_wrapper.h"

using namespace Graphics;

void Init(GContext* context)
{

}

void Render();
void RenderImGui();

int main()
{
    Wrapper::RunApplicationLoop(&Init, &Render, &RenderImGui);
}

void Render()
{

}

void RenderImGui()
{

}