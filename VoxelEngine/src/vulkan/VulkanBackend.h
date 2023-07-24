#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <core/Window.h>
#include <core/Scene.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan.h>
#include <components/camera/Camera.h>

namespace vulkan
{
	constexpr static int MAX_FRAMES_IN_FLIGHT = 2;
	static uint32 CURRENT_FRAME = 0;
	
	constexpr bool hasStencilComponent(const VkFormat& format);
	const std::vector<const char*> getRequiredExtensions();	
	constexpr const VkFormat findSupportedFormat(const VkPhysicalDevice& physicalDevice, const std::vector<VkFormat>& candidates, const VkImageTiling& tiling, const VkFormatFeatureFlags& features);
	constexpr const VkFormat& findDepthFormat(const VkPhysicalDevice& physicalDevice);
	void createInstance();
	const VkSurfaceKHR createSurface(const VkInstance& instance, GLFWwindow* window);
	void createRenderPass(const VkDevice& logicalDevice, const VkFormat& swapChainImageFormat);
	void createGraphicsPipeline(const VkDevice& logicalDevice,
		const VkRenderPass& renderPass,
		const VkDescriptorSetLayout& descriptorSetLayout);
	void createCommandPool(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice);
	void createCommandBuffers();
	void createSyncObjects(const VkDevice& logicalDevice);
	void createDescriptorPool(const VkDevice& logicalDevice);
	void createDescriptorSetLayout(const VkDevice& logicalDevice);
	void recreateSwapChain(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkSurfaceKHR& surface, GLFWwindow* window);
	void recordCommandBuffer(const VkCommandBuffer& commandBuffer, const uint32& imageIndex, const VoxelEngine::Scene* scene);
	void submitToQueue(const VkQueue& queue, const VkCommandBuffer& commandBuffer, const VkSemaphore* signalSemaphores = nullptr);
	void cleanupSwapChain(const VkDevice& logicalDevice, const VkSwapchainKHR& swapchain);
	void presentFrame(const uint32& imageIndex, VkSemaphore* signalSemaphores);
	void initImGui();
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	void setWindow(const VoxelEngine::Window& window);
	void setCamera(VoxelEngine::components::camera::Camera* camera);
	void init();
	void beginFrame();
	void endFrame();
	void deviceWaitIdle();
	void cleanup();
	void render(const VoxelEngine::Scene* scene);

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
	const VkDeviceMemory allocateMemory(const VkMemoryRequirements& requirements, const VkMemoryPropertyFlags& properties);
	void createBuffer(
		const VkDeviceSize& size,
		const VkBufferUsageFlags& usage,
		const VkMemoryPropertyFlags& properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory);
	void copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const VkDeviceSize& size);
	void endSingleTimeCommands(const VkCommandBuffer& commandBuffer);
	void destroyBuffer(const VkBuffer& buffer);
	void freeDeviceMemory(const VkDeviceMemory& memory);
	// ===================================================================
}