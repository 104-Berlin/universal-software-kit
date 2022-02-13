#pragma once

namespace Engine {

    class E_API ESerializer
    {
    public:
        static EJson WriteStorageDescriptionToJson(const EValueDescription& description);

        static EJson WriteSceneToJson(EDataBase* scene);

        static EJson WritePropertyToJs(EProperty* property, bool writeDescription = false);
        
        static ESharedBuffer WriteFullSceneBuffer(EDataBase* reg);

        static EJson WritEResourceBaseToJson(EResourceBase* data, bool writeData);
    };

}