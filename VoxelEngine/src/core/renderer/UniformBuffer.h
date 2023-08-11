#pragma once
#include <glm/mat4x4.hpp>

namespace VoxelEngine::renderer
{
	struct UniformBufferObject
	{
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		alignas(16) glm::mat4 viewproj;
		glm::vec3 lightPos;
	};
}