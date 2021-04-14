#include "engine.h"
#include "prefix_editor.h"


#ifdef M_PI
#undef M_PI
#endif

#define M_PI 3.14159f

namespace Engine {

	ECamera::ECamera(const glm::mat4& projectionMatrix)
		: fProjectionMatrix(projectionMatrix)
	{
	}

	void ECamera::UpdateImGui()
	{
	}
}
