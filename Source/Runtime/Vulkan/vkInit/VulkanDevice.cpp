#include "VulkanDevice.h"
#include "../vkUtils/VulkanStatistics.h"
#include "../vkUtils/VulkanValidation.h"

namespace vkInit
{
	constexpr const char* physicalDeviceTypeString(const VkPhysicalDeviceType& type)
	{
		switch (type)
		{
#define STR(r) case VK_PHYSICAL_DEVICE_TYPE_ ##r: return #r
			STR(OTHER);
			STR(INTEGRATED_GPU);
			STR(DISCRETE_GPU);
			STR(VIRTUAL_GPU);
			STR(CPU);
#undef STR
		default: return "UNKNOWN_DEVICE_TYPE";
		}
	}

	bool checkDeviceExtensionSupport(const VkPhysicalDevice& device, const bool& enableValidation)
	{
		uint32 extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		if (enableValidation && !availableExtensions.empty())
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
	
	bool isDeviceSuitable(const VkPhysicalDevice& device, const vkUtils::QueueFamilyIndices& queueFamilyIndices)
	{
		const bool extensionsSupported = checkDeviceExtensionSupport(device, vkUtils::_enableValidationLayers);

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		if (!supportedFeatures.geometryShader)
			return false;

		return queueFamilyIndices.isComplete() && extensionsSupported
			&& supportedFeatures.samplerAnisotropy
			&& supportedFeatures.fillModeNonSolid
			&& supportedFeatures.pipelineStatisticsQuery;
	}
	
	void setupPhysicalDevice(const VkInstance& instance, VulkanDevice* vulkanDevice)
	{
		uint32 deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		VOXEL_CORE_ASSERT(deviceCount != 0, "failed to find GPUs with Vulkan support!");

		std::vector<VkPhysicalDevice> candidates(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, candidates.data());

		for (const auto& physicalDevice : candidates)
		{
			vkUtils::QueueFamilyIndices queueFamilyIndices = vkUtils::findQueueFamilies(physicalDevice, vulkanDevice->surface);

			if (isDeviceSuitable(physicalDevice, queueFamilyIndices))
			{
				VkPhysicalDeviceProperties deviceProperties;
				vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

				VkPhysicalDeviceMemoryProperties memProperties;
				vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

				vulkanDevice->queueFamilyIndices = queueFamilyIndices;
				vulkanDevice->properties = deviceProperties;
				vulkanDevice->limits = deviceProperties.limits;
				vulkanDevice->physicalDevice = physicalDevice;
				vulkanDevice->memoryProperties = memProperties;

				VOXEL_CORE_TRACE("Physical device candidate: {0}.", deviceProperties.deviceName);
				VOXEL_CORE_TRACE("Device vendor ID: {0}.", deviceProperties.vendorID);
				VOXEL_CORE_TRACE("Device type: {0}.", physicalDeviceTypeString(deviceProperties.deviceType));
				VOXEL_CORE_TRACE("Device ID: {0}.", deviceProperties.deviceID);
				VOXEL_CORE_TRACE("Device hardware concurrency: {0}.", getHardwareConcurrency());
				return;
			}
		}
		throw std::runtime_error("failed to find a suitable GPU!");
	}
	
	const VkDevice createLogicalDevice(const VulkanDevice* vulkanDevice)
	{
		VOXEL_CORE_ASSERT(vulkanDevice->physicalDevice, "failed to create logical device!");

		float queuePriority = 1.0f;
		const uint32 graphicsFamilyIndex = vulkanDevice->queueFamilyIndices.graphicsFamily.value();

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
		VkResult err = vkCreateDevice(vulkanDevice->physicalDevice, &createInfo, nullptr, &device);
		VK_CHECK(err, "failed to create logical device!");
		return device;
	}
	
	const DeviceQueues getDeviceQueues(const VulkanDevice* device)
	{
		VOXEL_CORE_TRACE("Device graphics family: {0}.", device->queueFamilyIndices.graphicsFamily.value());
		VOXEL_CORE_TRACE("Device present family: {0}.", device->queueFamilyIndices.presentFamily.value());

		DeviceQueues queues;
		vkGetDeviceQueue(device->logicalDevice, device->queueFamilyIndices.graphicsFamily.value(), 0, &queues.graphicsQueue);
		vkGetDeviceQueue(device->logicalDevice, device->queueFamilyIndices.presentFamily.value(), 0, &queues.presentQueue);
		return queues;
	}
	
	VulkanDevice::VulkanDevice(const VkInstance& instance, const VkSurfaceKHR& surface)
		: surface(surface)
	{
		setupPhysicalDevice(instance, this);
		logicalDevice = createLogicalDevice(this);
		deviceQueues = getDeviceQueues(this);
		queryPool = new vkUtils::VulkanQueryStatisticsPool(this->logicalDevice);
	}

	VulkanDevice::~VulkanDevice()
	{
		delete queryPool;
		vkDestroyDevice(logicalDevice, nullptr);
	}
}