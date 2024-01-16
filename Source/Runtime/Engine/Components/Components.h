#pragma once
#include <pch.h>

namespace VoxelEngine::components
{
	struct TransformComponent
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() noexcept = default;
		TransformComponent(const TransformComponent&) noexcept = default;
		TransformComponent(const glm::vec3& position)
			: position(position) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(rotation));
			return glm::translate(glm::mat4(1.0f), position)
				* rotation
				* glm::scale(glm::mat4(1.0f), scale);
		}
	};

	struct CameraComponent
	{
		bool fixedAspectRatio = false;

		CameraComponent() noexcept = default;
		CameraComponent(const CameraComponent&) noexcept = default;
	};

	struct MeshComponent
	{
		
	};
}