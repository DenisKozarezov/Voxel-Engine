#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <pch.h>
#include <core/Window.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanUniformBuffer.h"

namespace VoxelEngine::renderer
{
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

	const std::vector<Vertex> vertices = 
	{
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

		{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
	};

	const std::vector<uint16> indices = 
	{
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};

	class VulkanRenderer
	{
		static SharedRef<VulkanRenderer> _singleton;		
		GLFWwindow* _window;
		VkInstance _instance;
		VkDebugUtilsMessengerEXT _debugMessenger;
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
		VkPipelineLayout _pipelineLayout;
		VkDescriptorSetLayout _descriptorSetLayout;
		VkDescriptorPool _descriptorPool;
		std::vector<VkDescriptorSet> _descriptorSets;
		VkPipeline _graphicsPipeline;
		VkImage _depthImage;
		VkDeviceMemory _depthImageMemory;
		VkImageView _depthImageView;
		VkFormat _swapChainImageFormat;
		VkExtent2D _swapChainExtent;
		IndexBuffer _indexBuffer;
		VertexBuffer _vertexBuffer;
		std::vector<UniformBuffer> _uniformBuffers;
		std::vector<VkImage> _swapChainImages;
		std::vector<VkImageView> _swapChainImageViews;
		std::vector<VkFramebuffer> _swapChainFramebuffers;
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
		const int rateDeviceSuitability(const VkPhysicalDevice& device) const;
		const std::vector<const char*> getRequiredExtensions() const;
		const VkResult createDebugUtilsMessengerEXT(const VkInstance& instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) const;
		const VkShaderModule createShaderModule(const std::vector<char>& code) const;
		constexpr VkDebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo() const;
		const QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device) const;
		const SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device) const;
		constexpr const VkSurfaceFormatKHR& chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
		constexpr const VkPresentModeKHR& chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
		constexpr const VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
		constexpr const VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, const VkImageTiling& tiling, const VkFormatFeatureFlags& features) const;
		constexpr const VkFormat& findDepthFormat() const;
		const void createInstance();
		const void createLogicalDevice();
		const void createSurface();
		const void createSwapChain();
		const void createRenderPass();
		const void createGraphicsPipeline();
		const void createFramebuffers();
		const void createUniformBuffers();
		const void createImageViews();
		const void createCommandPool();
		const void createCommandBuffers();
		const void createSyncObjects();
		const void createDescriptorSetLayout();
		const void createDescriptorPool();
		const void createDescriptorSets();
		const void setupDebugMessenger();
		const void recreateSwapChain();
		const void pickPhysicalDevice();
		const void recordCommandBuffer(const VkCommandBuffer& commandBuffer, const uint32& imageIndex) const;
		const void endSingleTimeCommands(const VkCommandBuffer& commandBuffer, const VkSemaphore* signalSemaphores = nullptr);
		const void destroyDebugUtilsMessengerEXT(const VkInstance& instance, const VkDebugUtilsMessengerEXT& debugMessenger, const VkAllocationCallbacks* pAllocator) const;
		const void cleanupSwapChain() const;
		const void cleanupUniformBuffers() const;
		const void presentFrame(const uint32& imageIndex, VkSemaphore* signalSemaphores);
		const void initImGui() const;
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	public:
		static const SharedRef<VulkanRenderer> getInstance();
		const uint32 findMemoryType(const uint32& typeFilter, const VkMemoryPropertyFlags& properties) const;
		inline const VkDevice& getLogicalDevice() const & { return _logicalDevice; }
		inline const VkPhysicalDevice& getPhysicalDevice() const & { return _physicalDevice; }
		inline const VkCommandBuffer& getCommandBuffer() const & { return _commandBuffers[_currentFrame]; }
		const void copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const VkDeviceSize& size) const;
		const void createBuffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
		const void setWindow(const Window& window);
		const void init();
		const void beginFrame();
		const void endFrame();
		const void deviceWaitIdle() const;
		const void cleanup() const;
	};
}