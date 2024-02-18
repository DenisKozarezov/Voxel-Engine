#pragma once
#include <Vulkan/vkUtils/VulkanSwapChainFrame.h>
#include <Renderer/Renderer.h>

struct UniformBufferObject;

namespace vulkan
{
	using namespace VoxelEngine;

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
	void submitToQueue(const vkUtils::SwapChainFrame& frame, const VkQueue& queue);
	void cleanupSwapChain();
	void presentFrame(const vkUtils::SwapChainFrame& frame);
	void prepareFrame();
	void beginFrame(const UniformBufferObject& ubo);
	void endFrame();

	void resize(const uint16& width, const uint16& height);
	void setClearColor(const glm::vec4 color);
	void setViewport(const int32_t& x, const int32_t& y, const uint32& width, const uint32& height);
	void init(const Window& window);
	void initImGui();	
	void deviceWaitIdle();
	void cleanup();
	
	const RenderFrameStats& getFrameStats();

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
	
	// ==================== MEMORY ALLOC / DEALLOC ====================
	void copyBuffer(const vkUtils::memory::Buffer& srcBuffer, vkUtils::memory::Buffer& dstBuffer, const VkDeviceSize& size);
	// ===================================================================
}
