#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/utils/VulkanAlloc.h>

namespace VoxelEngine
{
	class Scene
	{
	public:
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> indices;

		vkUtils::memory::Buffer vertexBuffer;
	};
}