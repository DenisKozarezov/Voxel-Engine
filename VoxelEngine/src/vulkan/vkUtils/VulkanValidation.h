#pragma once
#include <vulkan/vulkan.h>
#include <core/PrimitiveTypes.h>
#include <core/Assert.h>

namespace vkUtils
{
#ifdef VOXEL_RELEASE
	constexpr bool _enableValidationLayers = false;
#else
	constexpr bool _enableValidationLayers = true;
#endif

	const std::vector<const char*> validationLayers =
	{
		"VK_LAYER_KHRONOS_validation"
	};

	constexpr std::string errorString(VkResult errorCode)
	{
		switch (errorCode)
		{
#define STR(r) case VK_ ##r: return #r
			STR(NOT_READY);
			STR(TIMEOUT);
			STR(EVENT_SET);
			STR(EVENT_RESET);
			STR(INCOMPLETE);
			STR(ERROR_OUT_OF_HOST_MEMORY);
			STR(ERROR_OUT_OF_DEVICE_MEMORY);
			STR(ERROR_INITIALIZATION_FAILED);
			STR(ERROR_DEVICE_LOST);
			STR(ERROR_MEMORY_MAP_FAILED);
			STR(ERROR_LAYER_NOT_PRESENT);
			STR(ERROR_EXTENSION_NOT_PRESENT);
			STR(ERROR_FEATURE_NOT_PRESENT);
			STR(ERROR_INCOMPATIBLE_DRIVER);
			STR(ERROR_TOO_MANY_OBJECTS);
			STR(ERROR_FORMAT_NOT_SUPPORTED);
			STR(ERROR_SURFACE_LOST_KHR);
			STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
			STR(SUBOPTIMAL_KHR);
			STR(ERROR_OUT_OF_DATE_KHR);
			STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
			STR(ERROR_VALIDATION_FAILED_EXT);
			STR(ERROR_INVALID_SHADER_NV);
#undef STR
		default:
			return "UNKNOWN_ERROR";
		}
	}

	static constexpr void check_vk_result(const VkResult& vkResult, const std::string& exceptionMsg)
	{
		std::string str;
		str += "[VULKAN] [" + errorString(vkResult) + "] ";
		str += exceptionMsg;
		VOXEL_CORE_ASSERT(vkResult == VK_SUCCESS, str)
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