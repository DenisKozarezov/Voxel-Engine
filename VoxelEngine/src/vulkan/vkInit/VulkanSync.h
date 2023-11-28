#pragma once
#include "VulkanInitializers.h"
#include "../vkUtils/VulkanValidation.h"

namespace vkInit
{
	const VkSemaphore createSemaphore(const vkInit::VulkanDevice& device) 
	{
		VkSemaphoreCreateInfo semaphoreInfo = semaphoreCreateInfo();
		VkSemaphore semaphore;
		VkResult err = vkCreateSemaphore(device.logicalDevice, &semaphoreInfo, nullptr, &semaphore);
		VK_CHECK(err, "failed to create semaphore!");
		return semaphore;
	}

	const VkFence createFence(const vkInit::VulkanDevice& device)
	{
		VkFenceCreateInfo fenceInfo = fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
		VkFence fence;
		VkResult err = vkCreateFence(device.logicalDevice, &fenceInfo, nullptr, &fence);
		VK_CHECK(err, "failed to create fence!");
		return fence;
	}

	INLINE void lockFences(const vkInit::VulkanDevice& device, const VkFence* fences, const uint32& fenceCount = 1)
	{
		vkWaitForFences(device.logicalDevice, fenceCount, fences, VK_TRUE, UINT64_MAX);
	}

	INLINE void resetFences(const vkInit::VulkanDevice& device, const VkFence* fences, const uint32& fenceCount = 1)
	{
		vkResetFences(device.logicalDevice, fenceCount, fences);
	}
}