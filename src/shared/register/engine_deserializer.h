#pragma once

namespace Engine {

    class E_API EDeserializer
    {
    public:
        static void ReadSceneFromJson(const EJson& json, EScene* saveToScene);

        static void ReadPropertyFromJson(const EJson& json, EProperty* property);
    };

}