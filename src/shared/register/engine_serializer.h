#pragma once

namespace Engine {

    class E_API ESerializer
    {
    public:
        static EJson WriteSceneToJson(ERegister* scene);

        static EJson WritePropertyToJs(EProperty* property);
    };

}