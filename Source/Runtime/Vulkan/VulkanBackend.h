#pragma once
#include <Vulkan/vkUtils/VulkanSwapChainFrame.h>
#include <Renderer/Renderer.h>

namespace VoxelEngine
{
	namespace renderer
	{
		struct UniformBufferObject;
	}
}

namespace vulkan
{
	using namespace VoxelEngine;
	namespace mesh = components::mesh;

	VkSurfaceKHR makeInstance();
	void makeDevice(const VkSurfaceKHR& surface);
	void makeSwapChain();
	void makeFramebuffers();
	void makeDescriptorSetLayout();
	void makeGraphicsPipeline();
	void makeFrameResources();
	void makeCommandBuffers();
	void finalizeSetup();

	void recreateSwapChain();
	void recordCommandBuffer(const VkCommandBuffer& commandBuffer);
	void submitToQueue(const VkQueue& queue, const VkCommandBuffer& commandBuffer, const VkSemaphore* signalSemaphores = nullptr);
	void cleanupSwapChain();
	void presentFrame(const uint32& imageIndex, VkSemaphore* signalSemaphores);
	void prepareFrame();
	void beginFrame(const renderer::UniformBufferObject& ubo);
	void endFrame();

	void resize(const uint32& width, const uint32& height);
	void setClearColor(const glm::vec4 color);
	void setWindow(const Window& window);
	void setViewport(const int32_t& x, const int32_t& y, const uint32& width, const uint32& height);
	void init();
	void initImGui();	
	void deviceWaitIdle();
	void cleanup();

	renderer::RenderSettings& getRenderSettings();
	const renderer::RenderFrameStats& getFrameStats();
	const renderer::ShaderStats& getShaderStats();
	void resetFrameStats();

	/// <summary>
	/// Returns the vulkan device currently in use.
	/// </summary>
	/// <returns></returns>
	const vkInit::VulkanDevice* getDevice();

	/// <summary>
	/// Returns the command buffer used in this frame in flight.
	/// </summary>
	/// <returns></returns>
	const VkCommandBuffer& getCommandBuffer();

	const vkUtils::SwapChainFrame& getCurrentFrame();

	/// <summary>
	/// Returns a pool of commands for subsequent allocation of command buffers.
	/// </summary>
	/// <returns></returns>
	const VkCommandPool& getCommandPool();

	// ==================== MEMORY ALLOC / DEALLOC ====================
	void copyBuffer(const vkUtils::memory::Buffer& srcBuffer, vkUtils::memory::Buffer& dstBuffer, const VkDeviceSize& size);
	void endSingleTimeCommands(const VkCommandBuffer& commandBuffer);

	void copyBufferToImage(vkUtils::memory::Buffer buffer, VkImage image, uint32 width, uint32 height);
	void copyImage(VkCommandPool cmdPool, VkImage srcImageId, VkImage dstImageId, uint32 width, uint32 height);
	void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
	// ===================================================================
}
