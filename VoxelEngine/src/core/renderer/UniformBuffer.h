#pragma once
#include <glm/mat4x4.hpp>

namespace VoxelEngine::renderer
{
	struct UniformBufferObject
	{
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		glm::vec3 lightPos;
	};

	struct RaymarchData
	{
		glm::vec2 resolution;
		glm::vec2 mousePos;
		float voxelSize;
	};
}