#include "VulkanRendererAPI.h"
#include "VulkanBackend.h"

namespace vulkan
{
	void VulkanRendererAPI::init(const VoxelEngine::Window& window)
	{
		vulkan::setWindow(window);
		vulkan::init();
	}
	void VulkanRendererAPI::setViewport(const uint32& x, const uint32& y, const uint32& width, const uint32& height)
	{
		vulkan::setViewport(x, y, width, height);
	}
	void VulkanRendererAPI::setClearColor(const glm::vec4& color)
	{
	}
	void VulkanRendererAPI::setLineWidth(const float& width)
	{
	}
}