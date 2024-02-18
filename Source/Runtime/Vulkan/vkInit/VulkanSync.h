#pragma once
#include "VulkanInitializers.h"
#include "../vkUtils/VulkanValidation.h"

namespace vkInit
{
	VkSemaphore createSemaphore(const VkDevice& device) 
	{
		VkSemaphoreCreateInfo semaphoreInfo = semaphoreCreateInfo();
		VkSemaphore semaphore;
		VkResult err = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore);
		VK_CHECK(err, "failed to create semaphore!");
		return semaphore;
	}

	VkFence createFence(const VkDevice& device)
	{
		VkFenceCreateInfo fenceInfo = fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
		VkFence fence;
		VkResult err = vkCreateFence(device, &fenceInfo, nullptr, &fence);
		VK_CHECK(err, "failed to create fence!");
		return fence;
	}

	INLINE void lockFences(const VkDevice& device, const VkFence* fences, const uint32& fenceCount = 1)
	{
		vkWaitForFences(device, fenceCount, fences, VK_TRUE, UINT64_MAX);
	}

	INLINE void resetFences(const VkDevice& device, const VkFence* fences, const uint32& fenceCount = 1)
	{
		vkResetFences(device, fenceCount, fences);
	}
}