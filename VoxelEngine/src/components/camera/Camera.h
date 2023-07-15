#pragma once
#include <glm/glm.hpp>

namespace VoxelEngine::components::camera
{
	enum class CameraMovement { Forward, Backward, Left, Right };

	struct CameraUniformObject
	{
		glm::mat4 view;
		glm::mat4 proj;
		glm::mat4 viewproj;
	};
}