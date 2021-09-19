#pragma once

namespace Engine {

    class E_API EDeserializer
    {
    public:
        static bool ReadStorageDescriptionFromJson(const EJson& json, EValueDescription* description);


        static bool ReadSceneFromJson(const EJson& json, ERegister* saveToScene, const EVector<EValueDescription>& registeredTypes);

        static bool ReadPropertyFromJson(const EJson& json, EProperty* property);

        static bool ReadSceneFromFileBuffer(ESharedBuffer buffer, ERegister* saveToScene, const EVector<EValueDescription>& registeredTypes);
    };

}