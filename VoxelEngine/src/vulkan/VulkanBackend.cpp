#define GLFW_EXPOSE_NATIVE_WIN32
#include "VulkanBackend.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include "VulkanInitializers.h"
#include "VulkanTexture.h"
#include "assets_management/AssetsProvider.h"

namespace vulkan
{	
	struct VulkanState
	{
		GLFWwindow* window;
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugUtilsMessenger;
		VkDebugReportCallbackEXT debugReportMessenger;
		VkDevice logicalDevice;
		VkPhysicalDevice physicalDevice;
		VkCommandPool commandPool;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkPipelineCache pipelineCache;
		QueueFamilyIndices queueFamilyIndices;
		VkSurfaceKHR surface;
		VkSwapchainKHR swapChain;
		VkRenderPass renderPass;
		VkDescriptorPool descriptorPool;
		VkPipelineLayout pipelineLayout;
		VkDescriptorSetLayout descriptorSetLayout;
		VkPipeline graphicsPipeline;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<Framebuffer> swapChainFramebuffers;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		bool framebufferResized = false;
	} state;

	VulkanTexture* texture;
	VoxelEngine::components::camera::Camera* FPVcamera;
		
	void framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		state.framebufferResized = true;
	}
	const bool checkDeviceExtensionSupport(const VkPhysicalDevice& device)
	{
		uint32 extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		if (_enableValidationLayers && availableExtensions.size() > 0)
		{
			std::stringstream ss;
			for (const VkExtensionProperties& extension : availableExtensions)
			{
				ss << '\t';
				ss << extension.extensionName;
				ss << '\n';
			}
			VOXEL_CORE_TRACE("[VULKAN] Device available extensions:\n" + ss.str())
		}

		std::set<std::string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());
		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}
		return requiredExtensions.empty();
	}
	const bool isDeviceSuitable(const VkPhysicalDevice& device)
	{
		bool swapChainAdequate = false;
		bool extensionsSupported = checkDeviceExtensionSupport(device);

		if (extensionsSupported)
		{
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, state.surface);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return findQueueFamilies(device).isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}
	constexpr bool hasStencilComponent(const VkFormat& format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}	
	const std::vector<const char*> getRequiredExtensions()
	{
		uint32 glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
		if (_enableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

			if (extensions.size() > 0)
			{
				std::stringstream ss;
				for (const char* extensionName : extensions)
				{
					ss << '\t';
					ss << extensionName;
					ss << '\n';
				}
				VOXEL_CORE_TRACE("[VULKAN] Device extensions to be requested:\n" + ss.str())
			}
		}

		return extensions;
	}
	const QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device)
	{
		QueueFamilyIndices indices;

		uint32 queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, state.surface, &presentSupport);

			if (presentSupport) indices.presentFamily = i;

			if (indices.isComplete()) break;

			++i;
		}

		return indices;
	}
	const SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface)
	{
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32 formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32 presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}
		return details;
	}
	constexpr const VkSurfaceFormatKHR& chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}
		return availableFormats[0];
	}
	constexpr const VkPresentModeKHR& chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	constexpr const VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			int width, height;
			glfwGetFramebufferSize(state.window, &width, &height);

			VkExtent2D actualExtent =
			{
				static_cast<uint32>(width),
				static_cast<uint32>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
			return actualExtent;
		}
	}
	constexpr const VkFormat findSupportedFormat(const VkPhysicalDevice& physicalDevice, const std::vector<VkFormat>& candidates, const VkImageTiling& tiling, const VkFormatFeatureFlags& features)
	{
		for (const VkFormat& format : candidates) 
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			const bool isTilingLinear = tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features;
			const bool isTilingOptimal = tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features;
			if (isTilingLinear) return format;
			else if (isTilingOptimal) return format;
		}
		throw std::runtime_error("failed to find supported format!");
	}
	constexpr const VkFormat& findDepthFormat(const VkPhysicalDevice& physicalDevice)
	{
		return findSupportedFormat(physicalDevice,
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}	
	void createInstance()
	{
		bool layersSupported = _enableValidationLayers && checkValidationLayerSupport();
		VOXEL_CORE_ASSERT(layersSupported, "validation layers requested, but not available!")

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Voxel Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (_enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32>(_validationLayers.size());
			createInfo.ppEnabledLayerNames = _validationLayers.data();			
			VkDebugReportCallbackCreateInfoEXT debugCreateInfo = {};
			debugCreateInfo = populateDebugReportCreateInfo();
			createInfo.pNext = (VkDebugReportCallbackCreateInfoEXT*)&debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		VkResult err = vkCreateInstance(&createInfo, nullptr, &state.instance);
		check_vk_result(err, "failed to create instance!");
	}
	void createLogicalDevice(const VkPhysicalDevice& physicalDevice)
	{
		state.queueFamilyIndices = findQueueFamilies(physicalDevice);

		float queuePriority = 1.0f;
		uint32 graphicsFamilyIndex = state.queueFamilyIndices.graphicsFamily.value();
		uint32 presentFamilyIndex = state.queueFamilyIndices.presentFamily.value();
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = graphicsFamilyIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32>(_deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = _deviceExtensions.data();

		if (_enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32>(_validationLayers.size());
			createInfo.ppEnabledLayerNames = _validationLayers.data();
		}
		else createInfo.enabledLayerCount = 0;

		VkResult err = vkCreateDevice(physicalDevice, &createInfo, nullptr, &state.logicalDevice);
		check_vk_result(err, "failed to create logical device!");

		vkGetDeviceQueue(state.logicalDevice, graphicsFamilyIndex, 0, &state.graphicsQueue);
		vkGetDeviceQueue(state.logicalDevice, presentFamilyIndex, 0, &state.presentQueue);
	}
	void createSurface(const VkInstance& instance, GLFWwindow* window)
	{
		VkResult err = glfwCreateWindowSurface(instance, window, nullptr, &state.surface);
		check_vk_result(err, "failed to create window surface!");
	}
	void createSwapChain(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkSurfaceKHR& surface)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);
		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		uint32 imageCount = std::clamp(swapChainSupport.capabilities.minImageCount + 1, (uint32)0, swapChainSupport.capabilities.maxImageCount);

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		uint32 queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;						// Optional
			createInfo.pQueueFamilyIndices = nullptr;					// Optional
		}

		VkResult err = vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &state.swapChain);
		check_vk_result(err, "failed to create swap chain!");

		vkGetSwapchainImagesKHR(logicalDevice, state.swapChain, &imageCount, nullptr);
		state.swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(logicalDevice, state.swapChain, &imageCount, state.swapChainImages.data());

		state.swapChainImageFormat = surfaceFormat.format;
		state.swapChainExtent = extent;
	}
	void createRenderPass(const VkDevice& logicalDevice, const VkFormat& swapChainImageFormat)
	{
		VkAttachmentDescription colorAttachment = vulkan::initializers::renderPassColorAttachment(swapChainImageFormat);

		VkAttachmentDescription depthAttachment = vulkan::initializers::renderPassDepthAttachment(findDepthFormat(state.physicalDevice));

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::vector<VkAttachmentDescription> attachments = { colorAttachment, depthAttachment };
		std::vector<VkSubpassDescription> subpasses = { subpass };
		std::vector<VkSubpassDependency> dependencies = { dependency };
		VkRenderPassCreateInfo renderPassInfo = vulkan::initializers::renderPassCreateInfo(attachments, subpasses, dependencies);

		VkResult err = vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &state.renderPass);
		check_vk_result(err, "failed to create render pass!");
	}
	void createImageViews(const VkFormat& imageFormat)
	{
		state.swapChainImageViews.resize(state.swapChainImages.size());

		for (size_t i = 0; i < state.swapChainImages.size(); i++)
		{
			VkComponentMapping components =
			{
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY
			};
			state.swapChainImageViews[i] = memory::createImageView(state.logicalDevice, state.swapChainImages[i], imageFormat, components);
		}
	}
	void createDepthResources(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice)
	{
		VkFormat depthFormat = findDepthFormat(physicalDevice);
		state.depthImage = memory::createImage(physicalDevice, logicalDevice, state.swapChainExtent.width, state.swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, state.depthImageMemory);
		state.depthImageView = memory::createImageView(logicalDevice, state.depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	}
	void createGraphicsPipeline(
		const VkDevice& logicalDevice, 
		const VkRenderPass& renderPass, 
		const VkDescriptorSetLayout& descriptorSetLayout)
	{
		shaders::VulkanShader vertexShader = shaders::VulkanShader("resources/shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		shaders::VulkanShader fragShader = shaders::VulkanShader("resources/shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { vertexShader.getStage(), fragShader.getStage() };
		const auto& bindingDescription = Vertex::getBindingDescription();
		const auto& attributeDescription = Vertex::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = vulkan::initializers::pipelineVertexInputStateCreateInfo(
			&bindingDescription,
			1,
			attributeDescription.data(),
			static_cast<uint32>(attributeDescription.size()));

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = vulkan::initializers::pipelineInputAssemblyStateCreateInfo(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			VK_FALSE);

		VkPipelineViewportStateCreateInfo viewportState = vulkan::initializers::pipelineViewportStateCreateInfo(1, 1);

		VkPipelineRasterizationStateCreateInfo rasterizer = vulkan::initializers::pipelineRasterizationStateCreateInfo(
			VK_POLYGON_MODE_FILL, 
			VK_CULL_MODE_BACK_BIT, 
			VK_FRONT_FACE_COUNTER_CLOCKWISE);

		VkPipelineMultisampleStateCreateInfo multisampling = vulkan::initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);

		VkPipelineDepthStencilStateCreateInfo depthStencil = vulkan::initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS);

		VkPipelineColorBlendAttachmentState colorBlendAttachment = vulkan::initializers::pipelineColorBlendAttachmentState(
			VK_COLOR_COMPONENT_R_BIT | 
			VK_COLOR_COMPONENT_G_BIT | 
			VK_COLOR_COMPONENT_B_BIT | 
			VK_COLOR_COMPONENT_A_BIT, 
			VK_FALSE);
		VkPipelineColorBlendStateCreateInfo colorBlending = vulkan::initializers::pipelineColorBlendStateCreateInfo(colorBlendAttachment);

		std::vector<VkDynamicState> dynamicStates = 
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = vulkan::initializers::pipelineDynamicStateCreateInfo(dynamicStates);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = vulkan::initializers::pipelineLayoutCreateInfo(&descriptorSetLayout);

		VkResult err = vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &state.pipelineLayout);
		check_vk_result(err, "failed to create pipeline layout!");

		VkGraphicsPipelineCreateInfo pipelineInfo = vulkan::initializers::pipelineCreateInfo();
		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.layout = state.pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		err = vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &state.graphicsPipeline);
		check_vk_result(err, "failed to create graphics pipeline!");

		vertexShader.unbind();
		fragShader.unbind();
	}
	void createFramebuffers(const VkDevice& logicalDevice, const VkRenderPass& renderPass, const VkExtent2D& swapChainExtent)
	{
		size_t size = state.swapChainImageViews.size();

		state.swapChainFramebuffers.resize(size);
		for (size_t i = 0; i < size; i++)
		{
			std::vector<VkImageView> attachments =
			{
				state.swapChainImageViews[i],
				state.depthImageView
			};
			state.swapChainFramebuffers[i] = Framebuffer(logicalDevice, renderPass, attachments, swapChainExtent);
		}
	}
	void createCommandPool(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice)
	{
		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

		VkCommandPoolCreateInfo poolInfo = vulkan::initializers::commandPoolCreateInfo(queueFamilyIndices.graphicsFamily.value(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		VkResult err = vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &state.commandPool);
		check_vk_result(err, "failed to create command pool!");
	}
	void createCommandBuffers()
	{
		state.commandBuffers = memory::CommandBuffer::allocate(MAX_FRAMES_IN_FLIGHT);
	}
	void createSyncObjects(const VkDevice& logicalDevice)
	{
		state.imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		state.renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		state.inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo = vulkan::initializers::semaphoreCreateInfo();
		VkFenceCreateInfo fenceInfo = vulkan::initializers::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkResult err1 = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &state.imageAvailableSemaphores[i]);
			VkResult err2 = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &state.renderFinishedSemaphores[i]);
			VkResult err3 = vkCreateFence(logicalDevice, &fenceInfo, nullptr, &state.inFlightFences[i]);

			check_vk_result(err1, "failed to create semaphore!");
			check_vk_result(err2, "failed to create semaphore!");
			check_vk_result(err3, "failed to create fence!");
		}
	}	
	void initImGui()
	{
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = state.instance;
		init_info.PhysicalDevice = state.physicalDevice;
		init_info.Device = state.logicalDevice;
		init_info.QueueFamily = state.queueFamilyIndices.graphicsFamily.value();
		init_info.Queue = state.graphicsQueue;
		init_info.PipelineCache = state.pipelineCache;
		init_info.DescriptorPool = state.descriptorPool;
		init_info.Subpass = 0;
		init_info.MinImageCount = MAX_FRAMES_IN_FLIGHT;
		init_info.ImageCount = MAX_FRAMES_IN_FLIGHT;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = nullptr;
		ImGui_ImplGlfw_InitForVulkan(state.window, true);
		ImGui_ImplVulkan_Init(&init_info, state.renderPass);
		
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.Fonts->AddFontDefault();
		{
			// Use any command queue
			vkResetCommandPool(state.logicalDevice, state.commandPool, 0);
			memory::CommandBuffer::beginCommand(state.commandBuffers[CURRENT_FRAME]);

			ImGui_ImplVulkan_CreateFontsTexture(state.commandBuffers[CURRENT_FRAME]);

			VkSubmitInfo end_info = {};
			end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			end_info.commandBufferCount = 1;
			end_info.pCommandBuffers = &state.commandBuffers[CURRENT_FRAME];
			memory::CommandBuffer::endCommand(state.commandBuffers[CURRENT_FRAME]);
			vkQueueSubmit(state.graphicsQueue, 1, &end_info, VK_NULL_HANDLE);

			deviceWaitIdle();
			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}
	}
	void createDescriptorPool(const VkDevice& logicalDevice)
	{
		const auto& pool_sizes = vulkan::initializers::descriptorPoolSize();

		uint32 size = static_cast<uint32>(pool_sizes.size());
		VkDescriptorPoolCreateInfo poolInfo = vulkan::initializers::descriptorPoolCreateInfo(
			pool_sizes.data(), 
			size, 
			1000 * size, 
			VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);

		VkResult err = vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &state.descriptorPool);
		check_vk_result(err, "failed to create descriptor pool!");
	}
	void createDescriptorSetLayout(const VkDevice& logicalDevice)
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding = vulkan::initializers::descriptorSetLayoutBinding(
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 
			VK_SHADER_STAGE_VERTEX_BIT, 
			0,
			1);

		VkDescriptorSetLayoutBinding samplerLayoutBinding = vulkan::initializers::descriptorSetLayoutBinding(
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 
			VK_SHADER_STAGE_FRAGMENT_BIT, 
			1, 
			1);

		std::vector<VkDescriptorSetLayoutBinding> bindings = { uboLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo = vulkan::initializers::descriptorSetLayoutCreateInfo(bindings);

		VkResult err = vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &state.descriptorSetLayout);
		check_vk_result(err, "failed to create descriptor set layout!");
	}
	void recreateSwapChain(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkSurfaceKHR& surface, GLFWwindow* window)
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(window, &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(window, &width, &height);
			glfwWaitEvents();
		}
		deviceWaitIdle();

		cleanupSwapChain(logicalDevice, state.swapChain);
		createSwapChain(physicalDevice, logicalDevice, surface);
		createImageViews(state.swapChainImageFormat);
		createFramebuffers(logicalDevice, state.renderPass, state.swapChainExtent);
	}
	void pickPhysicalDevice(const VkInstance& instance)
	{
		uint32 deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		
		VOXEL_CORE_ASSERT(deviceCount != 0, "failed to find GPUs with Vulkan support!")

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices) 
		{
			if (isDeviceSuitable(device)) 
			{
				state.physicalDevice = device;
				break;
			}
		}

		if (state.physicalDevice == VK_NULL_HANDLE)
			throw std::runtime_error("failed to find a suitable GPU!");
	}
	void cleanupSwapChain(const VkDevice& logicalDevice, const VkSwapchainKHR& swapchain)
	{
		for (const auto& framebuffer : state.swapChainFramebuffers)
		{
			framebuffer.release();
		}
		for (const auto& imageView : state.swapChainImageViews)
		{
			memory::destroyImageView(logicalDevice, imageView);
		}
		vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);
	}
	void presentFrame(const uint32& imageIndex, VkSemaphore* signalSemaphores)
	{
		VkSwapchainKHR swapChains[] = { state.swapChain };

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		VkResult err = vkQueuePresentKHR(state.presentQueue, &presentInfo);

		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR || state.framebufferResized)
		{
			state.framebufferResized = false;
			recreateSwapChain(state.physicalDevice, state.logicalDevice, state.surface, state.window);
		}
		else check_vk_result(err, "failed to present swap chain image!");
	}
	void beginFrame() 
	{
		vkWaitForFences(state.logicalDevice, 1, &state.inFlightFences[CURRENT_FRAME], VK_TRUE, UINT64_MAX);
		uint32 imageIndex;
		VkResult result = vkAcquireNextImageKHR(state.logicalDevice, state.swapChain, UINT64_MAX, state.imageAvailableSemaphores[CURRENT_FRAME], VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain(state.physicalDevice, state.logicalDevice, state.surface, state.window);
			return;
		}
		else
			VOXEL_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "failed to acquire swap chain image!")

		VoxelEngine::renderer::UniformBufferObject ubo = {};
		const float aspectRatio = (float)state.swapChainExtent.width / state.swapChainExtent.height;

		ubo.model = glm::mat4(1.0f);
		ubo.view = FPVcamera->viewMatrix();
		ubo.proj = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 200.0f);
		ubo.proj[1][1] *= -1;

		texture->setUniformBuffer(&ubo, sizeof(ubo));

		vkResetFences(state.logicalDevice, 1, &state.inFlightFences[CURRENT_FRAME]);
		memory::CommandBuffer::reset(state.commandBuffers[CURRENT_FRAME]);

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();	
	}
	void endFrame()
	{
		ImGui::Render();

		recordCommandBuffer(state.renderPass, state.commandBuffers[CURRENT_FRAME], state.swapChainExtent, CURRENT_FRAME);

		VkSemaphore signalSemaphores[] = { state.renderFinishedSemaphores[CURRENT_FRAME] };
		submitToQueue(state.graphicsQueue, state.commandBuffers[CURRENT_FRAME], signalSemaphores);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		presentFrame(CURRENT_FRAME, signalSemaphores);

		CURRENT_FRAME = (CURRENT_FRAME + 1) % MAX_FRAMES_IN_FLIGHT;
	}
	void recordCommandBuffer(const VkRenderPass& renderPass, const VkCommandBuffer& commandBuffer, const VkExtent2D& swapChainExtent, const uint32& imageIndex)
	{
		memory::CommandBuffer::beginCommand(commandBuffer);

		std::vector<VkClearValue> clearValues(2);
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };
		VkRenderPassBeginInfo renderPassBeginInfo = vulkan::initializers::renderPassBeginInfo(
			renderPass, 
			state.swapChainFramebuffers[imageIndex],
			swapChainExtent,
			clearValues);
		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, state.graphicsPipeline);
		
		VkViewport viewport = vulkan::initializers::viewport(swapChainExtent, 0.0f, 1.0f);
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor = vulkan::initializers::rect2D(swapChainExtent, { 0, 0 });
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		// =================== RENDER WHOLE STUFF HERE ! ===================
		texture->render();

		ImDrawData* main_draw_data = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(main_draw_data, commandBuffer);
		// =================================================================

		vkCmdEndRenderPass(commandBuffer);
		memory::CommandBuffer::endCommand(commandBuffer);
	}
	void submitToQueue(const VkQueue& graphicsQueue, const VkCommandBuffer& commandBuffer, const VkSemaphore* signalSemaphores)
	{
		VkSemaphore waitSemaphores[] = { state.imageAvailableSemaphores[CURRENT_FRAME] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo = vulkan::initializers::submitInfo(
			waitSemaphores, 
			signalSemaphores, 
			commandBuffer, 
			waitStages);	

		VkResult err = vkQueueSubmit(graphicsQueue, 1, &submitInfo, state.inFlightFences[CURRENT_FRAME]);
		check_vk_result(err, "failed to submit draw command buffer! Possible reasons:\n 1) Incorrect shaders for submitting to the command queue. Please recompile your shaders!\n 2) Synchronization issues.");
	}
	void setWindow(const VoxelEngine::Window& window)
	{
		state.window = (GLFWwindow*)(window.getNativeWindow());
		glfwSetFramebufferSizeCallback(state.window, framebufferResizeCallback);
		int success = glfwVulkanSupported();
		VOXEL_CORE_ASSERT(success, "GLFW: Vulkan Not Supported")
	}
	void setCamera(VoxelEngine::components::camera::Camera* camera)
	{
		FPVcamera = camera;
	}

	void init()
	{		
		createInstance();
		setupDebugReportMessenger(state.instance, &state.debugReportMessenger);
		createSurface(state.instance, state.window);
		pickPhysicalDevice(state.instance);
		createLogicalDevice(state.physicalDevice);
		createSwapChain(state.physicalDevice, state.logicalDevice, state.surface);
		createImageViews(state.swapChainImageFormat);
		createRenderPass(state.logicalDevice, state.swapChainImageFormat);
		createDescriptorSetLayout(state.logicalDevice);
		createGraphicsPipeline(state.logicalDevice, state.renderPass, state.descriptorSetLayout);
		createCommandPool(state.physicalDevice, state.logicalDevice);
		createDepthResources(state.physicalDevice, state.logicalDevice);
		createFramebuffers(state.logicalDevice, state.renderPass, state.swapChainExtent);
		createDescriptorPool(state.logicalDevice);
		createCommandBuffers();
		createSyncObjects(state.logicalDevice);
		initImGui();

		TextureCreateInfo createInfo =
		{			
			state.logicalDevice,
			state.physicalDevice,
			state.pipelineLayout,
			state.descriptorSetLayout,
			state.descriptorPool,
			state.swapChainExtent
		};
		texture = new VulkanTexture(ASSET_PATH("textures/texture.jpg"), createInfo);	
	}
	void deviceWaitIdle()
	{
		vkDeviceWaitIdle(state.logicalDevice);
	}
	void cleanup()
	{
		texture->release();

		cleanupSwapChain(state.logicalDevice, state.swapChain);
		vkDestroyPipeline(state.logicalDevice, state.graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(state.logicalDevice, state.pipelineLayout, nullptr);
		vkDestroyRenderPass(state.logicalDevice, state.renderPass, nullptr);
		vkDestroyDescriptorPool(state.logicalDevice, state.descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(state.logicalDevice, state.descriptorSetLayout, nullptr);
		
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(state.logicalDevice, state.renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(state.logicalDevice, state.imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(state.logicalDevice, state.inFlightFences[i], nullptr);
		}

		memory::CommandBuffer::release(state.commandBuffers);
		vkDestroyCommandPool(state.logicalDevice, state.commandPool, nullptr);
		vkDestroyDevice(state.logicalDevice, nullptr);

		if (_enableValidationLayers)
		{
			destroyDebugReportMessengerEXT(state.instance, state.debugReportMessenger, nullptr);
		}

		vkDestroySurfaceKHR(state.instance, state.surface, nullptr);
		vkDestroyInstance(state.instance, nullptr);
	}

	const VkDevice& getLogicalDevice()
	{
		return state.logicalDevice;
	}
	const VkPhysicalDevice& getPhysicalDevice()
	{
		return state.physicalDevice;
	}
	const VkCommandBuffer& getCommandBuffer()
	{
		return state.commandBuffers[CURRENT_FRAME];
	}
	const VkCommandPool& getCommandPool()
	{
		return state.commandPool;
	}

	// ==================== MEMORY ALLOC / DEALLOC ====================
	const VkDeviceMemory allocateMemory(const VkMemoryRequirements& requirements, const VkMemoryPropertyFlags& properties)
	{
		return memory::allocateMemory(state.physicalDevice, state.logicalDevice, requirements, properties);
	}
	void createBuffer(
		const VkDeviceSize& size,
		const VkBufferUsageFlags& usage,
		const VkMemoryPropertyFlags& properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory)
	{
		memory::createBuffer(
			state.physicalDevice,
			state.logicalDevice,
			size,
			usage,
			properties,
			buffer,
			bufferMemory);
	}
	void destroyBuffer(const VkBuffer& buffer)
	{
		memory::destroyBuffer(state.logicalDevice, buffer);
	}
	void freeDeviceMemory(const VkDeviceMemory& memory)
	{
		memory::freeDeviceMemory(state.logicalDevice, memory);
	}
	void endSingleTimeCommands(const VkCommandBuffer& commandBuffer)
	{
		memory::CommandBuffer::endCommand(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(state.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(state.graphicsQueue);

		memory::CommandBuffer::release(commandBuffer);
	}
	void copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const VkDeviceSize& size)
	{
		VkCommandBuffer commandBuffer = memory::beginSingleTimeCommands();

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		endSingleTimeCommands(commandBuffer);
	}
	// =============================================================
}