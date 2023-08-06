#pragma once
#include <core/renderer/Renderer.h>
#include <components/mesh/Mesh.h>
#include <GLFW/glfw3.h>

namespace vulkan
{
	using namespace VoxelEngine;

	void makeInstance();
	void makeDevice();
	void makeSwapChain();
	void makeFramebuffers();
	void makeDescriptorSetLayout();
	void makeGraphicsPipeline();
	void makeFrameResources(const VkDevice& logicalDevice);
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

	void setWindow(const Window& window);
	void setCamera(const components::camera::Camera& camera);
	void submitRenderables(const std::vector<glm::vec3> objects);
	void init();
	void initImGui();	
	void deviceWaitIdle();
	void cleanup();

	renderer::RenderSettings& getRenderSettings();
	const renderer::RenderFrameStats& getFrameStats();
	void resetFrameStats();

	void prepareInstanceData(const std::vector<glm::vec3>& vertices);
	void makeAssets();
	void renderSceneObjects(
		const VkCommandBuffer& commandBuffer,
		const components::mesh::MeshType& objectType,
		uint32& startInstance,
		const uint32& instanceCount);

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