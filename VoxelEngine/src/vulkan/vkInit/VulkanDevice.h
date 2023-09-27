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

	INLINE const unsigned int getHardwareConcurrency() noexcept
	{
		return std::thread::hardware_concurrency();
	}

	const bool checkDeviceExtensionSupport(
		const VkPhysicalDevice& device,
		const bool& enableValidation);

	const bool isDeviceSuitable(
		const VkPhysicalDevice& device,
		const vkUtils::QueueFamilyIndices& queueFamilyIndices);

	const VkPhysicalDevice pickPhysicalDevice(
		const VkInstance& instance,
		const VkSurfaceKHR& surface,
		VkPhysicalDeviceLimits* limits);

	const VkDevice createLogicalDevice(
		const VkPhysicalDevice& physicalDevice,
		const VkSurfaceKHR& surface,
		const vkUtils::QueueFamilyIndices& queueFamilyIndices);

	const DeviceQueues getDeviceQueues(
		const VkPhysicalDevice& physicalDevice,
		const VkDevice& logicalDevice,
		const VkSurfaceKHR& surface);
}