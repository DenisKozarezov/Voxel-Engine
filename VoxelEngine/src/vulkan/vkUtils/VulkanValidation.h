#pragma once
#include <core/utils/EnumUtils.h>
#include <core/Assert.h>

#define VK_CHECK(result, msg) if (result != VK_SUCCESS) { VOXEL_CORE_ERROR("[VULKAN] [{0}] {1}", errorString(result), msg); VOXEL_DEBUGBREAK(); }

namespace vkUtils
{
#define VK_LAYER_VALIDATION_EXT_NAME "VK_LAYER_KHRONOS_validation"

#ifdef VOXEL_RELEASE
	constexpr bool _enableValidationLayers = false;
#else
	constexpr bool _enableValidationLayers = true;
#endif

	constexpr std::array<const char*, 1> validationLayers =
	{
		VK_LAYER_VALIDATION_EXT_NAME
	};

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