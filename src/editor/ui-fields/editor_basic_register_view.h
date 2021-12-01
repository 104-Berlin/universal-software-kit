#pragma once

namespace Editor {

    class EBasicRegisterView : public Engine::EUIField
    {
    public:
        EBasicRegisterView();

    private:
        ERef<EUIField> CreateTaskView();
        ERef<EUIField> CreateResourceView();
    };

}