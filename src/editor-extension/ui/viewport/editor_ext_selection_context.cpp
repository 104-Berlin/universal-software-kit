#include "editor_extension.h"

using namespace Engine;

ESelectionContext::ESelectionContext()
    : fSelectedObject(nullptr)
{

}

Renderer::RObject* ESelectionContext::GetSelectedObject() const
{
    return fSelectedObject;
}

void ESelectionContext::SetSelectedObject(Renderer::RObject* object)
{
    if (fSelectedObject == object) { return; }
    fSelectedObject = object;
}