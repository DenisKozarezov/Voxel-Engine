#include "VulkanDevice.h"

namespace vkInit
{
	VulkanDevice::VulkanDevice(const VkPhysicalDevice& physicalDevice)
	{
		VOXEL_CORE_ASSERT(physicalDevice, "physical device is null!");
		this->physicalDevice = physicalDevice;

		vkGetPhysicalDeviceProperties(physicalDevice, &properties);
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		VOXEL_CORE_ASSERT(queueFamilyCount > 0, "queueFamilyCount is zero!");
		queueFamilyProperties.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

		uint32_t extCount = 0;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, nullptr);
		if (extCount > 0)
		{
			std::vector<VkExtensionProperties> extensions(extCount);
			if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
			{
				for (auto& ext : extensions)
				{
					supportedExtensions.push_back(ext.extensionName);
				}
			}
		}
	}
	VulkanDevice::~VulkanDevice()
	{
		if (logicalDevice)
		{
			vkDestroyDevice(logicalDevice, nullptr);
		}
	}
}