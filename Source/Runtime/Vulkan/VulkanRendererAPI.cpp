#include "VulkanRendererAPI.h"
#include "VulkanBackend.h"

namespace vulkan
{
	void VulkanRendererAPI::init(const VoxelEngine::Window& window)
	{
		vulkan::init(window);
	}
	void VulkanRendererAPI::setViewport(const uint32& x, const uint32& y, const uint32& width, const uint32& height)
	{
		vulkan::setViewport(x, y, width, height);
	}
	void VulkanRendererAPI::setClearColor(const glm::vec4 color)
	{
		vulkan::setClearColor(color);
	}
	void VulkanRendererAPI::setLineWidth(const float& width)
	{
		vkCmdSetLineWidth(getCommandBuffer(), width);
	}
	void VulkanRendererAPI::draw(uint32 vertexCount, uint32 instanceCount, uint32 startVertex, uint32 startInstance)
	{
		const VkCommandBuffer commandBuffer = vulkan::getCommandBuffer();
		vkCmdDraw(commandBuffer, vertexCount, instanceCount, startVertex, startInstance);
	}
	void VulkanRendererAPI::drawMeshIndexed(uint32 indexCount, uint32 instanceCount, uint32 startIndex, uint32 startInstance)
	{
		const VkCommandBuffer commandBuffer = vulkan::getCommandBuffer();
		vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, startIndex, 0, startInstance);
	}
}