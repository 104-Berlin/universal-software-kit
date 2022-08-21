#pragma once

namespace Engine {
    
    class E_EDEXAPI ESelectionContext
    {
    private:
        Renderer::RObject* fSelectedObject;
    public:
        ESelectionContext();

        Renderer::RObject* GetSelectedObject() const;
        void SetSelectedObject(Renderer::RObject* object);
    };

}