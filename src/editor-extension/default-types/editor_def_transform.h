#pragma once

namespace Editor {

    
    E_STORAGE_STRUCT(ETransform,
        (EVec3, Position),
        (EVec3, Rotation),
        (EVec3, Scale, {1.0, 1.0, 1.0})
    )

    namespace ETransformHelper
    {
        static EMat4 GetTransformMatrix(const ETransform& transform)
        {
            EMat4 localMatrix(1.0);
            localMatrix *= glm::translate(EMat4(1.0f), transform.Position);
            localMatrix *= glm::toMat4(EQuat(transform.Rotation));
            localMatrix *= glm::scale(EMat4(1.0f), transform.Scale);
            return localMatrix;
        }
    };

    

}