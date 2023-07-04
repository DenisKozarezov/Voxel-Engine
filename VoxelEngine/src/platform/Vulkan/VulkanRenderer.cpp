#define GLFW_EXPOSE_NATIVE_WIN32
#include "VulkanRenderer.h"
#include <core/Assert.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include "VulkanTexture.h"

namespace VoxelEngine::renderer
{
	SharedRef<VulkanRenderer> VulkanRenderer::_singleton = nullptr;
	bool VulkanRenderer::_framebufferResized = false;

	VulkanTexture* texture;

	static std::vector<char> readFile(const std::string& filename) 
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}
	VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code) 
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}

	void VulkanRenderer::check_vk_result(const VkResult& vkResult, const std::string& exceptionMsg)
	{
		std::stringstream ss;
		ss << "[VULKAN] [" << vkResult << "] ";
		ss << exceptionMsg;
		VOXEL_CORE_ASSERT(vkResult == VK_SUCCESS, ss.str())
	}
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT		messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT				messageType,
		const VkDebugUtilsMessengerCallbackDataEXT	*pCallbackData,
		void*										pUserData) noexcept
	{
		std::stringstream message;
		message << "[VULKAN] " << pCallbackData->pMessage << std::endl;

		switch (messageType)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			VOXEL_CORE_INFO(message.str())
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			VOXEL_CORE_TRACE(message.str())
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			VOXEL_CORE_WARN(message.str())
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			VOXEL_CORE_ERROR(message.str())
			break;
		}
		return VK_FALSE;
	}	
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallback(
		VkDebugReportFlagsEXT       flags,
		VkDebugReportObjectTypeEXT  objectType,
		uint64_t                    object,
		size_t                      location,
		int32_t                     messageCode,
		const char*					pLayerPrefix,
		const char*					pMessage,
		void*						pUserData) noexcept
	{
		std::stringstream message;
		message << "[VULKAN] " << pMessage << std::endl;

		switch (objectType)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			VOXEL_CORE_INFO(message.str())
				break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			VOXEL_CORE_TRACE(message.str())
				break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			VOXEL_CORE_WARN(message.str())
				break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			VOXEL_CORE_ERROR(message.str())
				break;
		}
		return VK_FALSE;
	}

	void VulkanRenderer::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		_framebufferResized = true;
	}
	const bool VulkanRenderer::checkValidationLayerSupport() const
	{
		uint32 layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : _validationLayers) 
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) 
			{
				if (strcmp(layerName, layerProperties.layerName) == 0) 
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound) 
			{
				return false;
			}
		}

		return true;
	}
	const bool VulkanRenderer::checkDeviceExtensionSupport(const VkPhysicalDevice& device) const
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
	const bool VulkanRenderer::isDeviceSuitable(const VkPhysicalDevice& device) const
	{
		bool swapChainAdequate = false;
		bool extensionsSupported = checkDeviceExtensionSupport(device);

		if (extensionsSupported)
		{
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return findQueueFamilies(device).isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}
	constexpr bool VulkanRenderer::hasStencilComponent(const VkFormat& format) const
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}	
	const std::vector<const char*> VulkanRenderer::getRequiredExtensions() const
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
	const QueueFamilyIndices VulkanRenderer::findQueueFamilies(const VkPhysicalDevice& device) const
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
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);

			if (presentSupport) indices.presentFamily = i;

			if (indices.isComplete()) break;

			++i;
		}

		return indices;
	}
	const SwapChainSupportDetails VulkanRenderer::querySwapChainSupport(const VkPhysicalDevice& device) const
	{
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);

		uint32 formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
		}

		uint32 presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}
	constexpr const VkSurfaceFormatKHR& VulkanRenderer::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const
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
	constexpr const VkPresentModeKHR& VulkanRenderer::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const
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
	constexpr const VkExtent2D VulkanRenderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			int width, height;
			glfwGetFramebufferSize(_window, &width, &height);

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
	constexpr const VkFormat VulkanRenderer::findSupportedFormat(const std::vector<VkFormat>& candidates, const VkImageTiling& tiling, const VkFormatFeatureFlags& features) const
	{
		for (const VkFormat& format : candidates) 
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &props);

			const bool isTilingLinear = tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features;
			const bool isTilingOptimal = tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features;
			if (isTilingLinear) return format;
			else if (isTilingOptimal) return format;
		}
		throw std::runtime_error("failed to find supported format!");
	}
	constexpr const VkFormat& VulkanRenderer::findDepthFormat() const
	{
		return findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}
	const VkResult VulkanRenderer::createDebugUtilsMessengerEXT(const VkInstance& instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) const
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsCallbackEXT");
		if (func != nullptr)
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
	const VkResult VulkanRenderer::createDebugReportMessengerEXT(const VkInstance& instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pDebugMessenger) const
	{
		auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
		if (func != nullptr)
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
	constexpr VkDebugUtilsMessengerCreateInfoEXT VulkanRenderer::populateDebugUtilsCreateInfo() const
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = &debugUtilsCallback;
		return createInfo;
	}
	constexpr VkDebugReportCallbackCreateInfoEXT VulkanRenderer::populateDebugReportCreateInfo() const
	{
		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		createInfo.pNext = nullptr;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		createInfo.pfnCallback = &debugReportCallback;
		createInfo.pUserData = nullptr;
		return createInfo;
	}
	void VulkanRenderer::createInstance()
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

		VkResult err = vkCreateInstance(&createInfo, _allocator, &_instance);
		check_vk_result(err, "failed to create instance!");
	}
	void VulkanRenderer::createLogicalDevice()
	{
		_queueFamilyIndices = findQueueFamilies(_physicalDevice);

		float queuePriority = 1.0f;
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = _queueFamilyIndices.graphicsFamily.value();
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

		VkResult err = vkCreateDevice(_physicalDevice, &createInfo, _allocator, &_logicalDevice);
		check_vk_result(err, "failed to create logical device!");

		vkGetDeviceQueue(_logicalDevice, _queueFamilyIndices.graphicsFamily.value(), 0, &_graphicsQueue);
		vkGetDeviceQueue(_logicalDevice, _queueFamilyIndices.presentFamily.value(), 0, &_presentQueue);
	}
	void VulkanRenderer::createSurface()
	{
		VkResult err = glfwCreateWindowSurface(_instance, _window, nullptr, &_surface);
		check_vk_result(err, "failed to create window surface!");
	}
	void VulkanRenderer::createSwapChain()
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_physicalDevice);
		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		uint32 imageCount = std::clamp(swapChainSupport.capabilities.minImageCount + 1, (uint32)0, swapChainSupport.capabilities.maxImageCount);

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = _surface;
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

		QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);
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

		VkResult err = vkCreateSwapchainKHR(_logicalDevice, &createInfo, _allocator, &_swapChain);
		check_vk_result(err, "failed to create swap chain!");

		vkGetSwapchainImagesKHR(_logicalDevice, _swapChain, &imageCount, nullptr);
		_swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(_logicalDevice, _swapChain, &imageCount, _swapChainImages.data());

		_swapChainImageFormat = surfaceFormat.format;
		_swapChainExtent = extent;
	}
	void VulkanRenderer::createRenderPass()
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = _swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		std::vector<VkAttachmentDescription> attachments = { colorAttachment };
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = attachments.size();
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VkResult err = vkCreateRenderPass(_logicalDevice, &renderPassInfo, _allocator, &_renderPass);
		check_vk_result(err, "failed to create render pass!");
	}
	void VulkanRenderer::createImageViews()
	{
		_swapChainImageViews.resize(_swapChainImages.size());

		for (size_t i = 0; i < _swapChainImages.size(); i++) 
		{
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = _swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = _swapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			VkResult err = vkCreateImageView(_logicalDevice, &createInfo, nullptr, &_swapChainImageViews[i]);
			check_vk_result(err, "failed to create image views!");
		}
	}
	void VulkanRenderer::createGraphicsPipeline()
	{
		VulkanShader vertexShader = VulkanShader("resources/shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		VulkanShader fragShader = VulkanShader("resources/shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShader.getStage(), fragShader.getStage() };
		const auto& bindingDescription = Vertex::getBindingDescription();
		const auto& attributeDescription = Vertex::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32>(attributeDescription.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;								// Optional
		rasterizer.depthBiasClamp = 0.0f;										// Optional
		rasterizer.depthBiasSlopeFactor = 0.0f;									// Optional

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;									// Optional
		multisampling.pSampleMask = nullptr;									// Optional
		multisampling.alphaToCoverageEnable = VK_FALSE;							// Optional
		multisampling.alphaToOneEnable = VK_FALSE;								// Optional

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;					// Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;			// Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;		// Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;					// Optional

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		std::vector<VkDynamicState> dynamicStates = 
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout;

		if (vkCreatePipelineLayout(_logicalDevice, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = _pipelineLayout;
		pipelineInfo.renderPass = _renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		VkResult err = vkCreateGraphicsPipelines(_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, _allocator, &_graphicsPipeline);
		check_vk_result(err, "failed to create graphics pipeline!");

		vertexShader.unbind();
		fragShader.unbind();
	}
	void VulkanRenderer::createBuffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkResult err = vkCreateBuffer(_logicalDevice, &bufferInfo, _allocator, &buffer);
		check_vk_result(err, "failed to create buffer!");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(_logicalDevice, buffer, &memRequirements);

		bufferMemory = allocateMemory(memRequirements, properties);

		vkBindBufferMemory(_logicalDevice, buffer, bufferMemory, 0);
	}
	void VulkanRenderer::createFramebuffers()
	{
		_swapChainFramebuffers.resize(_swapChainImageViews.size());

		for (size_t i = 0; i < _swapChainImageViews.size(); i++)
		{
			std::vector<VkImageView> attachments = 
			{
				_swapChainImageViews[i]
			};
			_swapChainFramebuffers[i] = Framebuffer(_logicalDevice, _renderPass, attachments, _swapChainExtent, _allocator);
		}
	}
	void VulkanRenderer::createCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(_physicalDevice);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		VkResult err = vkCreateCommandPool(_logicalDevice, &poolInfo, _allocator, &_commandPool);
		check_vk_result(err, "failed to create command pool!");
	}
	void VulkanRenderer::createCommandBuffers()
	{
		_commandBuffers = CommandBuffer::allocate(MAX_FRAMES_IN_FLIGHT);
	}
	void VulkanRenderer::createSyncObjects()
	{
		_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkResult err1 = vkCreateSemaphore(_logicalDevice, &semaphoreInfo, _allocator, &_imageAvailableSemaphores[i]);
			VkResult err2 = vkCreateSemaphore(_logicalDevice, &semaphoreInfo, _allocator, &_renderFinishedSemaphores[i]);
			VkResult err3 = vkCreateFence(_logicalDevice, &fenceInfo, _allocator, &_inFlightFences[i]);

			check_vk_result(err1, "failed to create semaphore!");
			check_vk_result(err2, "failed to create semaphore!");
			check_vk_result(err3, "failed to create fence!");
		}
	}	
	void VulkanRenderer::initImGui() const
	{
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = _instance;
		init_info.PhysicalDevice = _physicalDevice;
		init_info.Device = _logicalDevice;
		init_info.QueueFamily = _queueFamilyIndices.graphicsFamily.value();
		init_info.Queue = _graphicsQueue;
		init_info.PipelineCache = _pipelineCache;
		init_info.DescriptorPool = _descriptorPool;
		init_info.Subpass = 0;
		init_info.MinImageCount = MAX_FRAMES_IN_FLIGHT;
		init_info.ImageCount = MAX_FRAMES_IN_FLIGHT;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = _allocator;
		ImGui_ImplGlfw_InitForVulkan(_window, true);
		ImGui_ImplVulkan_Init(&init_info, _renderPass);
		
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.Fonts->AddFontDefault();
		{
			// Use any command queue
			vkResetCommandPool(_logicalDevice, _commandPool, 0);
			CommandBuffer::beginCommand(_commandBuffers[_currentFrame]);

			ImGui_ImplVulkan_CreateFontsTexture(_commandBuffers[_currentFrame]);

			VkSubmitInfo end_info = {};
			end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			end_info.commandBufferCount = 1;
			end_info.pCommandBuffers = &_commandBuffers[_currentFrame];
			CommandBuffer::endCommand(_commandBuffers[_currentFrame]);
			vkQueueSubmit(_graphicsQueue, 1, &end_info, VK_NULL_HANDLE);

			deviceWaitIdle();
			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}
	}
	void VulkanRenderer::createDescriptorPool()
	{
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.poolSizeCount = (uint32)IM_ARRAYSIZE(pool_sizes);
		poolInfo.pPoolSizes = pool_sizes;
		poolInfo.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);

		if (vkCreateDescriptorPool(_logicalDevice, &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}
	void VulkanRenderer::createDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		VkResult err = vkCreateDescriptorSetLayout(_logicalDevice, &layoutInfo, _allocator, &_descriptorSetLayout);
		check_vk_result(err, "failed to create descriptor set layout!");
	}
	const VkDeviceMemory VulkanRenderer::allocateMemory(const VkMemoryRequirements& requirements, const VkMemoryPropertyFlags& properties) const
	{
		VkDeviceMemory memory;

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = requirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(requirements.memoryTypeBits, properties);

		VkResult err = vkAllocateMemory(_logicalDevice, &allocInfo, nullptr, &memory);
		check_vk_result(err, "failed to allocate memory!");

		return memory;
	}
	void VulkanRenderer::copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const VkDeviceSize& size)
	{
		VkCommandBuffer commandBuffer = CommandBuffer::allocate();
		CommandBuffer::beginCommand(commandBuffer);

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		endSingleTimeCommands(commandBuffer);
	}
	void VulkanRenderer::setupDebugMessenger()
	{
		if (!_enableValidationLayers) return;

		VkDebugReportCallbackCreateInfoEXT createInfo = populateDebugReportCreateInfo();
		VkResult err = createDebugReportMessengerEXT(_instance, &createInfo, _allocator, &_debugReportMessenger);
		check_vk_result(err, "failed to set up debug messenger!");
	}
	void VulkanRenderer::recreateSwapChain()
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(_window, &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(_window, &width, &height);
			glfwWaitEvents();
		}
		deviceWaitIdle();

		cleanupSwapChain();
		createSwapChain();
		createImageViews();
		createFramebuffers();
	}
	void VulkanRenderer::pickPhysicalDevice()
	{
		uint32 deviceCount = 0;
		vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
		
		VOXEL_CORE_ASSERT(deviceCount != 0, "failed to find GPUs with Vulkan support!")

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

		for (const auto& device : devices) 
		{
			if (isDeviceSuitable(device)) 
			{
				_physicalDevice = device;
				break;
			}
		}

		if (_physicalDevice == VK_NULL_HANDLE)
			throw std::runtime_error("failed to find a suitable GPU!");
	}
	void VulkanRenderer::destroyDebugUtilsMessengerEXT(const VkInstance& instance, const VkDebugUtilsMessengerEXT& debugMessenger, const VkAllocationCallbacks* pAllocator) const
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}
	void VulkanRenderer::destroyDebugReportMessengerEXT(const VkInstance& instance, const VkDebugReportCallbackEXT& debugMessenger, const VkAllocationCallbacks* pAllocator) const
	{
		auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}
	void VulkanRenderer::cleanupSwapChain() const
	{
		for (const auto& framebuffer : _swapChainFramebuffers) 
		{
			framebuffer.release();
		}
		for (const auto& imageView : _swapChainImageViews)
		{
			vkDestroyImageView(_logicalDevice, imageView, _allocator);
		}
		vkDestroySwapchainKHR(_logicalDevice, _swapChain, _allocator);
	}
	void VulkanRenderer::presentFrame(const uint32& imageIndex, VkSemaphore* signalSemaphores)
	{
		VkSwapchainKHR swapChains[] = { _swapChain };

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		VkResult err = vkQueuePresentKHR(_presentQueue, &presentInfo);

		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR || _framebufferResized)
		{
			_framebufferResized = false;
			recreateSwapChain();
		}
		else check_vk_result(err, "failed to present swap chain image!");
	}
	void VulkanRenderer::beginFrame() 
	{
		vkWaitForFences(_logicalDevice, 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);
		uint32 imageIndex;
		VkResult result = vkAcquireNextImageKHR(_logicalDevice, _swapChain, UINT64_MAX, _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return;
		}
		else
			VOXEL_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "failed to acquire swap chain image!")

		texture->updateUniformBuffer(_currentFrame);

		vkResetFences(_logicalDevice, 1, &_inFlightFences[_currentFrame]);
		CommandBuffer::reset(_commandBuffers[_currentFrame]);

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();	
	}
	void VulkanRenderer::endFrame()
	{
		ImGui::Render();

		recordCommandBuffer(_commandBuffers[_currentFrame], _currentFrame);

		VkSemaphore signalSemaphores[] = { _renderFinishedSemaphores[_currentFrame] };
		submitToQueue(_commandBuffers[_currentFrame], signalSemaphores);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		presentFrame(_currentFrame, signalSemaphores);

		_currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}
	void VulkanRenderer::recordCommandBuffer(const VkCommandBuffer& commandBuffer, const uint32& imageIndex) const
	{
		CommandBuffer::beginCommand(commandBuffer);

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = _renderPass;
		renderPassInfo.framebuffer = _swapChainFramebuffers[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = _swapChainExtent;

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)_swapChainExtent.width;
		viewport.height = (float)_swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = _swapChainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		// =================== RENDER WHOLE STUFF HERE ! ===================
		texture->render(commandBuffer, _pipelineLayout, imageIndex);

		ImDrawData* main_draw_data = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(main_draw_data, commandBuffer);
		// =================================================================

		vkCmdEndRenderPass(commandBuffer);
		CommandBuffer::endCommand(commandBuffer);
	}
	void VulkanRenderer::submitToQueue(const VkCommandBuffer& commandBuffer, const VkSemaphore* signalSemaphores)
	{
		VkSemaphore waitSemaphores[] = { _imageAvailableSemaphores[_currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VkResult err = vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFences[_currentFrame]);
		check_vk_result(err, "failed to submit draw command buffer! Possible reasons:\n 1) Incorrect shaders for submitting to the command queue. Please recompile your shaders!\n 2) Synchronization issues.");
	}
	void VulkanRenderer::endSingleTimeCommands(const VkCommandBuffer& commandBuffer) const
	{
		CommandBuffer::endCommand(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(_graphicsQueue);

		CommandBuffer::release(commandBuffer);
	}
	const SharedRef<VulkanRenderer> VulkanRenderer::getInstance()
	{
		if (_singleton == nullptr)
		{
			_singleton = MakeShared<VulkanRenderer>();
		}
		return _singleton;
	}
	const uint32 VulkanRenderer::findMemoryType(const uint32& typeFilter, const VkMemoryPropertyFlags& properties) const
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);

		for (uint32 i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		throw std::runtime_error("failed to find suitable memory type!");
	}
	void VulkanRenderer::setWindow(const Window& window)
	{
		_window = (GLFWwindow*)(window.getNativeWindow());
		glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
		int success = glfwVulkanSupported();
		VOXEL_CORE_ASSERT(success, "GLFW: Vulkan Not Supported")
	}	
	const VkCommandBuffer& VulkanRenderer::beginSingleTimeCommands() const
	{
		VkCommandBuffer commandBuffer = CommandBuffer::allocate();
		CommandBuffer::beginCommand(commandBuffer);

		return commandBuffer;
	}	
	void VulkanRenderer::init()
	{		
		createInstance();
		setupDebugMessenger();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
		createRenderPass();
		createDescriptorSetLayout();
		createGraphicsPipeline();
		createFramebuffers();
		createCommandPool();
		createDescriptorPool();
		createCommandBuffers();
		createSyncObjects();
		initImGui();

		texture = new VulkanTexture("resources/textures/texture.jpg");
	}
	void VulkanRenderer::deviceWaitIdle() const
	{
		vkDeviceWaitIdle(_logicalDevice);
	}
	void VulkanRenderer::cleanup() const
	{
		texture->release();

		cleanupSwapChain();
		vkDestroyPipeline(_logicalDevice, _graphicsPipeline, _allocator);
		vkDestroyPipelineLayout(_logicalDevice, _pipelineLayout, _allocator);
		vkDestroyRenderPass(_logicalDevice, _renderPass, _allocator);
		vkDestroyDescriptorPool(_logicalDevice, _descriptorPool, _allocator);
		vkDestroyDescriptorSetLayout(_logicalDevice, _descriptorSetLayout, _allocator);
		
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(_logicalDevice, _renderFinishedSemaphores[i], _allocator);
			vkDestroySemaphore(_logicalDevice, _imageAvailableSemaphores[i], _allocator);
			vkDestroyFence(_logicalDevice, _inFlightFences[i], _allocator);
		}

		CommandBuffer::release(_commandBuffers);
		vkDestroyCommandPool(_logicalDevice, _commandPool, _allocator);
		vkDestroyDevice(_logicalDevice, _allocator);

		if (_enableValidationLayers)
		{
			destroyDebugReportMessengerEXT(_instance, _debugReportMessenger, _allocator);
		}

		vkDestroySurfaceKHR(_instance, _surface, _allocator);
		vkDestroyInstance(_instance, _allocator);
	}
}