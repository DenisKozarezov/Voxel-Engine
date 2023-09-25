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
	void VulkanRendererAPI::setClearColor(const glm::vec4 color)
	{
		vulkan::setClearColor(color);
	}
	void VulkanRendererAPI::setLineWidth(const float& width)
	{
		vkCmdSetLineWidth(getCommandBuffer(), width);
	}
	void VulkanRendererAPI::drawMesh(const components::mesh::Mesh& mesh)
	{
		VkCommandBuffer commandBuffer = vulkan::getCommandBuffer();
		vkCmdDraw(commandBuffer, mesh.vertexCount, 1, 0, 0);
	}
	void VulkanRendererAPI::drawMeshIndexed(uint32 indexCount, uint32 instanceCount, uint32 startIndex, uint32 startInstance)
	{
		VkCommandBuffer commandBuffer = vulkan::getCommandBuffer();
		vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, startIndex, 0, startInstance);
	}
}