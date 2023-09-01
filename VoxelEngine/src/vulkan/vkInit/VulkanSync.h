#pragma once
#include "VulkanInitializers.h"
#include "../vkUtils/VulkanValidation.h"

namespace vkInit
{
	const VkSemaphore createSemaphore(const VkDevice& logicalDevice) 
	{
		VkSemaphoreCreateInfo semaphoreInfo = semaphoreCreateInfo();

		VkSemaphore semaphore;
		VkResult err = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &semaphore);
		VK_CHECK(err, "failed to create semaphore!");
		return semaphore;
	}

	const VkFence createFence(const VkDevice& logicalDevice) 
	{
		VkFenceCreateInfo fenceInfo = fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);

		VkFence fence;
		VkResult err = vkCreateFence(logicalDevice, &fenceInfo, nullptr, &fence);
		VK_CHECK(err, "failed to create fence!");
		return fence;
	}

	void lockFences(const VkDevice& logicalDevice, const VkFence* fences, const uint32& fenceCount = 1)
	{
		vkWaitForFences(logicalDevice, fenceCount, fences, VK_TRUE, UINT64_MAX);
	}

	void resetFences(const VkDevice& logicalDevice, const VkFence* fences, const uint32& fenceCount = 1)
	{
		vkResetFences(logicalDevice, fenceCount, fences);
	}
}