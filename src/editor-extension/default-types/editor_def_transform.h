#pragma once

namespace Editor {

    
    E_STORAGE_STRUCT(ETransform,
        (EVec3, Position),
        (EVec3, Rotation),
        (EVec3, Scale, {1.0, 1.0, 1.0})
    )

    namespace ETransformHelper
    {
        static EMat4 GetMatrixFromTransform(const ETransform& transform)
        {
            EMat4 localMatrix(1.0);
            localMatrix *= glm::translate(EMat4(1.0f), transform.Position);
            localMatrix *= glm::toMat4(EQuat(transform.Rotation));
            localMatrix *= glm::scale(EMat4(1.0f), transform.Scale);
            return localMatrix;
        }

        static ETransform GetTransformFromMatrix(const EMat4& transformMatrix)
        {
            EVec3 position, rotation, scale;
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transformMatrix), glm::value_ptr(position), glm::value_ptr(rotation), glm::value_ptr(scale));
            rotation = EVec3(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z));

            return ETransform(position, rotation, scale);
        }
    };

    

}