#pragma once
#include <pch.h>
#include "../vkUtils/VulkanQueueFamilies.h"
#include "../vkUtils/VulkanValidation.h"

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

	const bool checkDeviceExtensionSupport(
		const VkPhysicalDevice& device,
		const bool& enableValidation)
	{
		uint32 extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		if (enableValidation && availableExtensions.size() > 0)
		{
			std::stringstream ss;
			for (const VkExtensionProperties& extension : availableExtensions)
			{
				ss << '\t';
				ss << extension.extensionName;
				ss << '\n';
			}
			VOXEL_CORE_TRACE("Device available extensions:\n" + ss.str())
		}

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}
		return requiredExtensions.empty();
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
	constexpr bool hasStencilComponent(const VkFormat& format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	const bool isDeviceSuitable(
		const VkPhysicalDevice& device,
		const VkSurfaceKHR& surface,
		const bool& enableValidation)
	{
		bool extensionsSupported = checkDeviceExtensionSupport(device, enableValidation);

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return vkUtils::findQueueFamilies(device, surface).isComplete() && extensionsSupported && supportedFeatures.samplerAnisotropy;
	}

	const VkPhysicalDevice pickPhysicalDevice(
		const VkInstance& instance, 
		const VkSurfaceKHR& surface, 
		const bool& enableValidation)
	{
		uint32 deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		VOXEL_CORE_ASSERT(deviceCount != 0, "failed to find GPUs with Vulkan support!")

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		VkPhysicalDevice physicalDevice;
		for (const auto& device : devices)
		{
			if (isDeviceSuitable(device, surface, enableValidation))
			{
				return device;
			}
		}
		throw std::runtime_error("failed to find a suitable GPU!");
	}

	const VkDevice createLogicalDevice(
		const VkPhysicalDevice& physicalDevice, 
		const VkSurfaceKHR& surface, 
		const bool& enableValidation)
	{
		vkUtils::QueueFamilyIndices queueFamilyIndices = vkUtils::findQueueFamilies(physicalDevice, surface);

		float queuePriority = 1.0f;
		uint32 graphicsFamilyIndex = queueFamilyIndices.graphicsFamily.value();
		uint32 presentFamilyIndex =queueFamilyIndices.presentFamily.value();
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
		createInfo.enabledExtensionCount = static_cast<uint32>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (enableValidation)
		{
			createInfo.enabledLayerCount = static_cast<uint32>(vkUtils::validationLayers.size());
			createInfo.ppEnabledLayerNames = vkUtils::validationLayers.data();
		}
		else createInfo.enabledLayerCount = 0;

		VkDevice device;
		VkResult err = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
		vkUtils::check_vk_result(err, "failed to create logical device!");
		return device;
	}

	const DeviceQueues getDeviceQueues(
		const VkPhysicalDevice& physicalDevice, 
		const VkDevice& logicalDevice, 
		const VkSurfaceKHR& surface)
	{
		vkUtils::QueueFamilyIndices queueFamilyIndices = vkUtils::findQueueFamilies(physicalDevice, surface);
		DeviceQueues queues;
		vkGetDeviceQueue(logicalDevice, queueFamilyIndices.graphicsFamily.value(), 0, &queues.graphicsQueue);
		vkGetDeviceQueue(logicalDevice, queueFamilyIndices.presentFamily.value(), 0, &queues.presentQueue);
		return queues;
	}

}