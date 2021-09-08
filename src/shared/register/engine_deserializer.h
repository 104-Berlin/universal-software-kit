#pragma once

namespace Engine {

    class E_API EDeserializer
    {
    public:
        static void ReadSceneFromJson(const EJson& json, ERegister* saveToScene, const EVector<EValueDescription>& registeredTypes);

        static void ReadPropertyFromJson(const EJson& json, EProperty* property);

        static void ReadSceneFromFileBuffer(ESharedBuffer buffer, ERegister* saveToScene, const EVector<EValueDescription>& registeredTypes);
    };

}