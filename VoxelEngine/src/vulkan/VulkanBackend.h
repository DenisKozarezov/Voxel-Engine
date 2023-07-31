#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <core/renderer/Renderer.h>
#include <GLFW/glfw3.h>
#include <components/mesh/Mesh.h>
#include "vkUtils/VulkanAlloc.h"

namespace vulkan
{
	using namespace VoxelEngine::components::mesh;

	void makeInstance();
	void makeDevice();
	void makeSwapChain();
	void makeFramebuffers();
	void makeDescriptorSetLayout();
	void makeGraphicsPipeline();
	void makeFrameResources(const VkDevice& logicalDevice);
	void makeCommandPool(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice);
	void makeCommandBuffers();
	void finalizeSetup();

	void recreateSwapChain(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkSurfaceKHR& surface, GLFWwindow* window);
	void recordCommandBuffer(const VkCommandBuffer& commandBuffer, const uint32& imageIndex);
	void submitToQueue(const VkQueue& queue, const VkCommandBuffer& commandBuffer, const VkSemaphore* signalSemaphores = nullptr);
	void cleanupSwapChain();
	void presentFrame(const uint32& imageIndex, VkSemaphore* signalSemaphores);
	void prepareFrame(const uint32& imageIndex);
	void prepareScene(const VkCommandBuffer& commandBuffer);
	void beginFrame();
	void endFrame();

	void setWindow(const VoxelEngine::Window& window);
	void setCamera(const VoxelEngine::components::camera::Camera* camera);
	void setScene(const VoxelEngine::Scene* scene);
	void init();
	void initImGui();	
	void deviceWaitIdle();
	void cleanup();

	VoxelEngine::renderer::RenderSettings& getRenderSettings();

	void makeAssets();
	void renderSceneObjects(
		const VkCommandBuffer& commandBuffer,
		const MeshType& objectType,
		uint32& startInstance,
		const uint32& instanceCount);

	std::tuple<const string*, uint64*> getPipelineStats();

	const VkDevice& getLogicalDevice();

	/// <summary>
	/// Returns the physical GPU device currently in use for Vulkan.
	/// </summary>
	/// <returns></returns>
	const VkPhysicalDevice& getPhysicalDevice();

	/// <summary>
	/// Returns the command buffer used in this frame in flight.
	/// </summary>
	/// <returns></returns>
	const VkCommandBuffer& getCommandBuffer();

	/// <summary>
	/// Returns a pool of commands for subsequent allocation of command buffers.
	/// </summary>
	/// <returns></returns>
	const VkCommandPool& getCommandPool();

	// ==================== MEMORY ALLOC / DEALLOC ====================
	void copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const VkDeviceSize& size);
	void endSingleTimeCommands(const VkCommandBuffer& commandBuffer);
	// ===================================================================
}