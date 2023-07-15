#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <core/Window.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <components/camera/Camera.h>
#include "VulkanFramebuffer.h"
#include "VulkanShader.h"
#include "VulkanCommandBuffer.h"
#include "VulkanUniformBuffer.h"
#include "VulkanValidation.h"
#include "VulkanAlloc.h"

namespace vulkan
{
	constexpr static int MAX_FRAMES_IN_FLIGHT = 2;
	static uint32 CURRENT_FRAME = 0;

	struct QueueFamilyIndices
	{
		std::optional<uint32> graphicsFamily;
		std::optional<uint32> presentFamily;

		constexpr bool isComplete() const noexcept
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	
	const bool isDeviceSuitable(const VkPhysicalDevice& device);
	constexpr bool hasStencilComponent(const VkFormat& format);
	const std::vector<const char*> getRequiredExtensions();
	const QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device);
	const SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);
	constexpr const VkSurfaceFormatKHR& chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	constexpr const VkPresentModeKHR& chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	constexpr const VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	constexpr const VkFormat findSupportedFormat(const VkPhysicalDevice& physicalDevice, const std::vector<VkFormat>& candidates, const VkImageTiling& tiling, const VkFormatFeatureFlags& features);
	constexpr const VkFormat& findDepthFormat();
	void createInstance();
	void createLogicalDevice(const VkPhysicalDevice& physicalDevice);
	void createSurface(const VkInstance& instance, GLFWwindow* window);
	void createSwapChain(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkSurfaceKHR& surface);
	void createRenderPass(const VkDevice& logicalDevice, const VkFormat& swapChainImageFormat);
	void createGraphicsPipeline(const VkDevice& logicalDevice,
		const VkRenderPass& renderPass,
		const VkDescriptorSetLayout& descriptorSetLayout);
	void createFramebuffers(const VkDevice& logicalDevice, 
		const VkRenderPass& renderPass, 
		const VkExtent2D& swapChainExtent);
	void createImageViews(const VkFormat& imageFormat);
	void createCommandPool(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice);
	void createCommandBuffers();
	void createSyncObjects(const VkDevice& logicalDevice);
	void createDescriptorPool(const VkDevice& logicalDevice);
	void createDescriptorSetLayout(const VkDevice& logicalDevice);
	void recreateSwapChain(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkSurfaceKHR& surface, GLFWwindow* window);
	void pickPhysicalDevice(const VkInstance& instance);
	void recordCommandBuffer(const VkRenderPass& renderPass, const VkCommandBuffer& commandBuffer, const VkExtent2D& swapChainExtent, const uint32& imageIndex);
	void submitToQueue(const VkQueue& graphicsQueue, const VkCommandBuffer& commandBuffer, const VkSemaphore* signalSemaphores = nullptr);
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