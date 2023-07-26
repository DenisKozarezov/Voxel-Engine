#pragma once
#include <optional>
#include "VulkanValidation.h"

namespace vkUtils
{
	struct QueueFamilyIndices
	{
		std::optional<uint32> graphicsFamily;
		std::optional<uint32> presentFamily;
		std::optional<uint32> computeFamily;

		constexpr bool isComplete() const noexcept
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	const QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface)
	{
		QueueFamilyIndices indices;

		uint32 queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		VOXEL_CORE_TRACE("There are {0} queue families available on the system.", queueFamilyCount)

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (presentSupport) indices.presentFamily = i;

			if (indices.isComplete()) break;

			++i;
		}
		return indices;
	}
}