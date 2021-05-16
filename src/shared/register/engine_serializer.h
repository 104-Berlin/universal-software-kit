#pragma once

namespace Engine {

    class E_API ESerializer
    {
    public:
        static EJson WriteSceneToJson(EScene* scene);

        static EJson WritePropertyToJs(EProperty* property);
    };

}