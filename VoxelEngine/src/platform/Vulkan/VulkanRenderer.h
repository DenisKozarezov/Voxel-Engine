#pragma once
#include <pch.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan.h>
#include <core/PrimitiveTypes.h>

namespace VoxelEngine::renderer
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription description = {};
			description.binding = 0;
			description.stride = sizeof(Vertex);
			description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return description;
		}
		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, position);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

			return attributeDescriptions;
		}
	};
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
	struct UniformBufferObject
	{
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 projection;
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
		VkInstance _instance;
		GLFWwindow* _window;
		VkDebugUtilsMessengerEXT _debugMessenger;
		static VkDevice _logicalDevice;
		static VkPhysicalDevice _physicalDevice;
		static VkCommandPool _commandPool;
		static VkQueue _graphicsQueue;
		VkQueue _presentQueue;
		VkSurfaceKHR _surface;
		VkSwapchainKHR _swapChain;
		VkFormat _swapChainImageFormat;
		VkExtent2D _swapChainExtent;
		VkRenderPass _renderPass;
		VkPipelineLayout _pipelineLayout;
		VkDescriptorSetLayout _descriptorSetLayout;
		VkDescriptorPool _descriptorPool;
		std::vector<VkDescriptorSet> _descriptorSets;
		VkPipeline _graphicsPipeline;
		VkBuffer _vertexBuffer;
		VkDeviceMemory _vertexBufferMemory;
		VkBuffer _indexBuffer;
		VkDeviceMemory _indexBufferMemory;
		VkImage _depthImage;
		VkDeviceMemory _depthImageMemory;
		VkImageView _depthImageView;
		std::vector<VkBuffer> _uniformBuffers;
		std::vector<VkDeviceMemory> _uniformBuffersMemory;
		std::vector<VkImage> _swapChainImages;
		std::vector<VkImageView> _swapChainImageViews;
		std::vector<VkFramebuffer> _swapChainFramebuffers;
		std::vector<VkCommandBuffer> _commandBuffers;
		std::vector<VkSemaphore> _imageAvailableSemaphores;
		std::vector<VkSemaphore> _renderFinishedSemaphores;
		std::vector<VkFence> _inFlightFences;
		uint32_t _currentFrame;

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
		const void createBuffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		const void createFramebuffers();
		const void createUniformBuffers();
		const void createVertexBuffer();
		const void createIndexBuffer();
		const void createCommandPool();
		const void createCommandBuffers();
		const void createSyncObjects();
		const void createDescriptorSetLayout();
		const void createDescriptorPool();
		const void createDescriptorSets();
		const void copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const VkDeviceSize& size);
		const void copyBufferToImage(const VkBuffer& buffer, const VkImage& image, const uint32& width, const uint32& height);
		const void setupDebugMessenger();
		const void recreateSwapChain();
		const void pickPhysicalDevice();
		const void destroyDebugUtilsMessengerEXT(const VkInstance& instance, const VkDebugUtilsMessengerEXT& debugMessenger, const VkAllocationCallbacks* pAllocator);
		const void cleanupSwapChain();
		const void cleanupUniformBuffers();
	public:
		static const VkCommandBuffer beginSingleTimeCommands();
		static const uint32 findMemoryType(const uint32& typeFilter, const VkMemoryPropertyFlags& properties);
		inline static const VkDevice& getLogicalDevice() { return _logicalDevice; }
		inline static const VkPhysicalDevice& getPhysicalDevice() { return _physicalDevice; }
		static const void endSingleTimeCommands(const VkCommandBuffer& commandBuffer);
		const void setGLFWwindow(GLFWwindow* const window) noexcept;
		const void init();
		const void renderFrame();
		const void deviceWaitIdle() const;
		const void cleanup();
	};
}