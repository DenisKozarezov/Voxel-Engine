#pragma once
#include <optional>
#include <Core/CoreTypes.h>
#include <vulkan/vulkan.hpp>

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

	const QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);
}