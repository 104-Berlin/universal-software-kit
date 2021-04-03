#pragma once

namespace Editor {

	class ECamera
	{
	public:
		ECamera(const glm::mat4& projectionMatrix);

		virtual void UpdateImGui();

		inline void SetProjectionMatrix(const glm::mat4& projectionMatrix) { fProjectionMatrix = projectionMatrix; }
		const glm::mat4& GetProjectionMatrix() const { return fProjectionMatrix; }
	private:
		glm::mat4 fProjectionMatrix;
	};


}