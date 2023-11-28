#pragma once
#include <pch.h>
#include <core/Base.h>
#include "../vkUtils/VulkanQueueFamilies.h"

namespace vkInit
{
	const std::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	struct DeviceQueues
	{
		VkQueue graphicsQueue;
		VkQueue presentQueue;
	};

	struct VulkanDevice
	{
		VkDevice logicalDevice = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		DeviceQueues deviceQueues;
		VkQueryPool queryPool = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceFeatures features;
		VkPhysicalDeviceMemoryProperties memoryProperties;
		VkPhysicalDeviceLimits limits;
		vkUtils::QueueFamilyIndices queueFamilyIndices;
		std::vector<VkQueueFamilyProperties> queueFamilyProperties;
		std::vector<string> supportedExtensions;

		VulkanDevice() noexcept = default;
		explicit VulkanDevice(const VkInstance& instance, const VkSurfaceKHR& surface);
		~VulkanDevice();

		void release();
	};

	INLINE const unsigned int getHardwareConcurrency() noexcept
	{
		return std::thread::hardware_concurrency();
	}

	const bool checkDeviceExtensionSupport(
		const VkPhysicalDevice& device,
		const bool& enableValidation);
}