#pragma once
#include <pch.h>
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

	inline const unsigned int getHardwareConcurrency() noexcept
	{
		return std::thread::hardware_concurrency();
	}

	struct VulkanDevice
	{
		VkDevice logicalDevice;
		VkPhysicalDevice physicalDevice;
		DeviceQueues deviceQueues;
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceFeatures features;
		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkUtils::QueueFamilyIndices queueFamilyIndices;
		std::vector<VkQueueFamilyProperties> queueFamilyProperties;
		std::vector<string> supportedExtensions;
		const unsigned int hardwareConcurrency = getHardwareConcurrency();

		constexpr operator const VkDevice&() const &
		{
			return logicalDevice;
		};
		explicit VulkanDevice(const VkPhysicalDevice& physicalDevice);
		~VulkanDevice();
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
				ss << '\t' << extension.extensionName << '\n';
			}
			VOXEL_CORE_TRACE("Device available extensions:\n" + ss.str());
		}

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}
		return requiredExtensions.empty();
	}

	const bool isDeviceSuitable(
		const VkPhysicalDevice& device,
		const vkUtils::QueueFamilyIndices& queueFamilyIndices)
	{
		bool extensionsSupported = checkDeviceExtensionSupport(device, vkUtils::_enableValidationLayers);

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		if (!supportedFeatures.geometryShader)
			return false;

		return queueFamilyIndices.isComplete() && extensionsSupported
			&& supportedFeatures.samplerAnisotropy
			&& supportedFeatures.fillModeNonSolid
			&& supportedFeatures.pipelineStatisticsQuery;
	}

	const VkPhysicalDevice pickPhysicalDevice(
		const VkInstance& instance, 
		const VkSurfaceKHR& surface,
		VkPhysicalDeviceLimits* limits)
	{
		uint32 deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		VOXEL_CORE_ASSERT(deviceCount != 0, "failed to find GPUs with Vulkan support!");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);

			vkUtils::QueueFamilyIndices queueFamilyIndices = vkUtils::findQueueFamilies(device, surface);

			if (isDeviceSuitable(device, queueFamilyIndices))
			{
				VOXEL_CORE_TRACE("Physical device candidate: {0}.", deviceProperties.deviceName);
				VOXEL_CORE_TRACE("Device vendor ID: {0}.", deviceProperties.vendorID);
				VOXEL_CORE_TRACE("Device type: {0}.", physicalDeviceTypeString(deviceProperties.deviceType));
				VOXEL_CORE_TRACE("Device ID: {0}.", deviceProperties.deviceID);
				VOXEL_CORE_TRACE("Device hardware concurrency: {0}.", getHardwareConcurrency());

				limits = &deviceProperties.limits;
				return device;
			}
		}
		throw std::runtime_error("failed to find a suitable GPU!");
	}

	const VkDevice createLogicalDevice(
		const VkPhysicalDevice& physicalDevice, 
		const VkSurfaceKHR& surface,
		const vkUtils::QueueFamilyIndices& queueFamilyIndices)
	{
		VOXEL_CORE_ASSERT(physicalDevice, "failed to create logical device!");

		float queuePriority = 1.0f;
		uint32 graphicsFamilyIndex = queueFamilyIndices.graphicsFamily.value();
		uint32 presentFamilyIndex = queueFamilyIndices.presentFamily.value();
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = graphicsFamilyIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.pipelineStatisticsQuery = VK_TRUE;
		deviceFeatures.fillModeNonSolid = VK_TRUE;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (vkUtils::_enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32>(vkUtils::validationLayers.size());
			createInfo.ppEnabledLayerNames = vkUtils::validationLayers.data();
		}
		else createInfo.enabledLayerCount = 0;

		VkDevice device;
		VkResult err = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
		VK_CHECK(err, "failed to create logical device!");
		return device;
	}

	const DeviceQueues getDeviceQueues(
		const VkPhysicalDevice& physicalDevice, 
		const VkDevice& logicalDevice, 
		const VkSurfaceKHR& surface)
	{
		vkUtils::QueueFamilyIndices queueFamilyIndices = vkUtils::findQueueFamilies(physicalDevice, surface);
		
		VOXEL_CORE_TRACE("Device graphics family: {0}.", queueFamilyIndices.graphicsFamily.value());
		VOXEL_CORE_TRACE("Device present family: {0}.", queueFamilyIndices.presentFamily.value());

		DeviceQueues queues;
		vkGetDeviceQueue(logicalDevice, queueFamilyIndices.graphicsFamily.value(), 0, &queues.graphicsQueue);
		vkGetDeviceQueue(logicalDevice, queueFamilyIndices.presentFamily.value(), 0, &queues.presentQueue);

		return queues;
	}
}