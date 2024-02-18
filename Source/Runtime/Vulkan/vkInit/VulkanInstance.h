#pragma once
#include <vector>
#include <Core/CoreTypes.h>
#include <vulkan/vulkan.h>

namespace VoxelEngine
{
	class Window;
}

namespace vkInit
{
	const std::vector<const char*> getRequiredExtensions();

	const VkInstance createInstance(const uint32& vulkanApi);

	const VkSurfaceKHR createSurface(const VkInstance& instance, const VoxelEngine::Window* window);
}
