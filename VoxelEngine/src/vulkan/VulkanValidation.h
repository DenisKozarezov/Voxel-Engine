#pragma once
#include <vulkan/vulkan.h>
#include <core/PrimitiveTypes.h>
#include <core/Assert.h>

namespace vulkan
{
#ifdef VOXEL_RELEASE
	constexpr bool _enableValidationLayers = false;
#else
	constexpr bool _enableValidationLayers = true;
#endif

	const std::vector<const char*> _validationLayers =
	{
		"VK_LAYER_KHRONOS_validation"
	};
	const std::vector<const char*> _deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	static void check_vk_result(const VkResult& vkResult, const std::string& exceptionMsg)
	{
		std::stringstream ss;
		ss << "[VULKAN] [" << vkResult << "] ";
		ss << exceptionMsg;
		VOXEL_CORE_ASSERT(vkResult == VK_SUCCESS, ss.str())
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT		messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT				messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) noexcept;

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallback(
		VkDebugReportFlagsEXT       flags,
		VkDebugReportObjectTypeEXT  objectType,
		uint64_t                    object,
		size_t                      location,
		int32_t                     messageCode,
		const char* pLayerPrefix,
		const char* pMessage,
		void* pUserData) noexcept;

	const VkResult createDebugUtilsMessengerEXT(
		const VkInstance& instance, 
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
		const VkAllocationCallbacks* pAllocator, 
		VkDebugUtilsMessengerEXT* pDebugMessenger);

	const VkResult createDebugReportMessengerEXT(
		const VkInstance& instance, 
		const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, 
		const VkAllocationCallbacks* pAllocator, 
		VkDebugReportCallbackEXT* pDebugMessenger);

	void destroyDebugUtilsMessengerEXT(
		const VkInstance& instance,
		const VkDebugUtilsMessengerEXT& debugMessenger,
		const VkAllocationCallbacks* pAllocator);

	void destroyDebugReportMessengerEXT(
		const VkInstance& instance,
		const VkDebugReportCallbackEXT& debugMessenger,
		const VkAllocationCallbacks* pAllocator);

	constexpr VkDebugUtilsMessengerCreateInfoEXT populateDebugUtilsCreateInfo();
	constexpr VkDebugReportCallbackCreateInfoEXT populateDebugReportCreateInfo();
	const bool checkValidationLayerSupport();
	void setupDebugReportMessenger(const VkInstance& instance, VkDebugReportCallbackEXT* debugReportFunc);	
}