#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <pch.h>
#include <core/Window.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "VulkanFramebuffer.h"
#include "VulkanShader.h"
#include "VulkanCommandBuffer.h"
#include "VulkanUniformBuffer.h"
#include "VulkanVertex.h"

namespace VoxelEngine::renderer
{
	constexpr static int MAX_FRAMES_IN_FLIGHT = 2;

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

	class VulkanRenderer
	{
	public:
		static SharedRef<VulkanRenderer> _singleton;		
		GLFWwindow* _window;
		VkInstance _instance;
		VkDebugUtilsMessengerEXT _debugUtilsMessenger;
		VkDebugReportCallbackEXT _debugReportMessenger;
		VkDevice _logicalDevice;
		VkPhysicalDevice _physicalDevice;
		VkCommandPool _commandPool;
		VkQueue _graphicsQueue;
		VkAllocationCallbacks* _allocator;
		VkQueue _presentQueue;
		VkPipelineCache _pipelineCache;
		QueueFamilyIndices _queueFamilyIndices;
		VkSurfaceKHR _surface;
		VkSwapchainKHR _swapChain;
		VkRenderPass _renderPass;
		VkDescriptorPool _descriptorPool;
		VkPipelineLayout _pipelineLayout;
		VkDescriptorSetLayout _descriptorSetLayout;
		VkPipeline _graphicsPipeline;
		VkFormat _swapChainImageFormat;
		VkExtent2D _swapChainExtent;
		std::vector<VkImage> _swapChainImages;
		std::vector<VkImageView> _swapChainImageViews;
		std::vector<Framebuffer> _swapChainFramebuffers;
		std::vector<VkCommandBuffer> _commandBuffers;
		std::vector<VkSemaphore> _imageAvailableSemaphores;
		std::vector<VkSemaphore> _renderFinishedSemaphores;
		std::vector<VkFence> _inFlightFences;
		uint32 _currentFrame = 0;
		static bool _framebufferResized;

#ifdef VOXEL_RELEASE
		const bool _enableValidationLayers = false;
#else
		const bool _enableValidationLayers = true;
#endif

		const std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
		};

		const std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0
		};


		const std::vector<const char*> _validationLayers =
		{
			"VK_LAYER_KHRONOS_validation"
		};
		const std::vector<const char*> _deviceExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		const bool checkValidationLayerSupport() const;
		const bool checkDeviceExtensionSupport(const VkPhysicalDevice& device) const;
		const bool isDeviceSuitable(const VkPhysicalDevice& device) const;
		constexpr bool hasStencilComponent(const VkFormat& format) const;
		const std::vector<const char*> getRequiredExtensions() const;
		const VkResult createDebugUtilsMessengerEXT(const VkInstance& instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) const;
		const VkResult createDebugReportMessengerEXT(const VkInstance& instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pDebugMessenger) const;
		constexpr VkDebugUtilsMessengerCreateInfoEXT populateDebugUtilsCreateInfo() const;
		constexpr VkDebugReportCallbackCreateInfoEXT populateDebugReportCreateInfo() const;
		const QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device) const;
		const SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device) const;
		constexpr const VkSurfaceFormatKHR& chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
		constexpr const VkPresentModeKHR& chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
		constexpr const VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
		constexpr const VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, const VkImageTiling& tiling, const VkFormatFeatureFlags& features) const;
		constexpr const VkFormat& findDepthFormat() const;
		const VkCommandBuffer& beginSingleTimeCommands() const;
		void createInstance();
		void createLogicalDevice();
		void createSurface();
		void createSwapChain();
		void createRenderPass();
		void createGraphicsPipeline();
		void createFramebuffers();
		void createImageViews();
		void createCommandPool();
		void createCommandBuffers();
		void createSyncObjects();
		void createDescriptorPool();
		void createDescriptorSetLayout();
		void setupDebugMessenger();
		void recreateSwapChain();
		void pickPhysicalDevice();
		void recordCommandBuffer(const VkCommandBuffer& commandBuffer, const uint32& imageIndex) const;
		void submitToQueue(const VkCommandBuffer& commandBuffer, const VkSemaphore* signalSemaphores = nullptr);
		void endSingleTimeCommands(const VkCommandBuffer& commandBuffer) const;
		void destroyDebugUtilsMessengerEXT(const VkInstance& instance, const VkDebugUtilsMessengerEXT& debugMessenger, const VkAllocationCallbacks* pAllocator) const;
		void destroyDebugReportMessengerEXT(const VkInstance& instance, const VkDebugReportCallbackEXT& debugMessenger, const VkAllocationCallbacks* pAllocator) const;
		void cleanupSwapChain() const;
		void presentFrame(const uint32& imageIndex, VkSemaphore* signalSemaphores);
		void initImGui() const;
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	public:
		static const SharedRef<VulkanRenderer> getInstance();
		static void check_vk_result(const VkResult& vkResult, const std::string& exceptionMsg);
		const uint32 findMemoryType(const uint32& typeFilter, const VkMemoryPropertyFlags& properties) const;
		inline const VkDevice& getLogicalDevice() const & { return _logicalDevice; }
		inline const VkPhysicalDevice& getPhysicalDevice() const & { return _physicalDevice; }
		inline const VkCommandBuffer& getCommandBuffer() const & { return _commandBuffers[_currentFrame]; }
		const VkCommandPool& getCommandPool() const& { return _commandPool; }
		const VkDeviceMemory allocateMemory(const VkMemoryRequirements& requirements, const VkMemoryPropertyFlags& properties) const;
		void copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const VkDeviceSize& size);
		void createBuffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
		void setWindow(const Window& window);
		void init();
		void beginFrame();
		void endFrame();
		void deviceWaitIdle() const;
		void cleanup() const;
	};
}