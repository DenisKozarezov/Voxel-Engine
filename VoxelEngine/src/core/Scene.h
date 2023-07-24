#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/VulkanAlloc.h>

namespace VoxelEngine
{
	class Scene
	{
	public:
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> indices;

		vulkan::memory::Buffer vertexBuffer;
	};
}