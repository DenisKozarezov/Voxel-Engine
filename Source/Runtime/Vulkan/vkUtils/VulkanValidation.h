#pragma once
#include <Core/pch.h>
#include <Core/Logging/Assert.h>
#include <vulkan/vulkan.h>

constexpr const char* errorString(const VkResult& errorCode)
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

	VkResult createDebugUtilsMessengerEXT(
		const VkInstance& instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger);

	VkResult createDebugReportMessengerEXT(
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
	bool checkValidationLayerSupport();
	void setupDebugReportMessenger(const VkInstance& instance, VkDebugReportCallbackEXT* debugReportFunc);	
}