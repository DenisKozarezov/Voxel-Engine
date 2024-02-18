#include "VulkanValidation.h"

namespace vkUtils
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT		messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT				messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) noexcept
	{
		std::stringstream message;
		message << "[VULKAN] " << pCallbackData->pMessage << std::endl;

		switch (messageType)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			RUNTIME_INFO(message.str());
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			RUNTIME_TRACE(message.str());
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			RUNTIME_WARN(message.str());
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			RUNTIME_ERROR(message.str());
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
		const char* pLayerPrefix,
		const char* pMessage,
		void* pUserData) noexcept
	{
		std::stringstream message;
		message << "[VULKAN] " << pMessage << std::endl;

		switch (flags)
		{
		case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
			RUNTIME_INFO(message.str());
			break;
		case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
			RUNTIME_TRACE(message.str());
			break;
		case VK_DEBUG_REPORT_WARNING_BIT_EXT:
			RUNTIME_WARN(message.str());
			break;
		case VK_DEBUG_REPORT_ERROR_BIT_EXT:
			RUNTIME_ERROR(message.str());
			break;
		}
		return VK_FALSE;
	}

	VkResult createDebugUtilsMessengerEXT(
		const VkInstance& instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsCallbackEXT");
		if (func != nullptr)
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	VkResult createDebugReportMessengerEXT(
		const VkInstance& instance,
		const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugReportCallbackEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
		if (func != nullptr)
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void destroyDebugUtilsMessengerEXT(
		const VkInstance& instance, 
		const VkDebugUtilsMessengerEXT& debugMessenger, 
		const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}

	void destroyDebugReportMessengerEXT(
		const VkInstance& instance, 
		const VkDebugReportCallbackEXT& debugMessenger, 
		const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}

	constexpr VkDebugUtilsMessengerCreateInfoEXT populateDebugUtilsCreateInfo()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = &debugUtilsCallback;
		return createInfo;
	}
	constexpr VkDebugReportCallbackCreateInfoEXT populateDebugReportCreateInfo()
	{
		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		createInfo.pNext = nullptr;
		createInfo.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT;
		createInfo.pfnCallback = &debugReportCallback;
		createInfo.pUserData = nullptr;
		return createInfo;
	}

	bool checkValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers)
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
	void setupDebugUtilsMessenger(const VkInstance& instance, VkDebugUtilsMessengerEXT* debugUtilsFunc)
	{
#ifndef ENABLE_VALIDATION_LAYERS
		return;
#endif
		
		const auto createInfo = populateDebugUtilsCreateInfo();
		VkResult err = createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, debugUtilsFunc);
		VK_CHECK(err, "failed to set up debug report messenger!");

		RUNTIME_TRACE("Vulkan debug report messenger set up.");
	}		
	void setupDebugReportMessenger(const VkInstance& instance, VkDebugReportCallbackEXT* debugReportFunc)
	{
#ifndef ENABLE_VALIDATION_LAYERS
		return;
#endif
		
		const auto createInfo = populateDebugReportCreateInfo();
		VkResult err = createDebugReportMessengerEXT(instance, &createInfo, nullptr, debugReportFunc);
		VK_CHECK(err, "failed to set up debug report messenger!");

		RUNTIME_TRACE("Vulkan debug report messenger set up.");
	}		
}