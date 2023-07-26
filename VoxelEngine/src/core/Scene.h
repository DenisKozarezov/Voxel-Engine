#pragma once
#include <vector>
#include <vulkan/VulkanVertex.h>

namespace VoxelEngine
{
	class Scene
	{
	public:
		std::vector<glm::vec3> vertices;

		Scene();
	};
}