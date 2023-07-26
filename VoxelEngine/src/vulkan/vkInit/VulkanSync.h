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
		vkUtils::check_vk_result(err, "failed to create semaphore!");
		return semaphore;
	}

	const VkFence createFence(const VkDevice& logicalDevice) 
	{
		VkFenceCreateInfo fenceInfo = fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);

		VkFence fence;
		VkResult err = vkCreateFence(logicalDevice, &fenceInfo, nullptr, &fence);
		vkUtils::check_vk_result(err, "failed to create fence!");
		return fence;
	}

	void destroySemaphore(const VkDevice& logicalDevice, const VkSemaphore& semaphore)
	{
		vkDestroySemaphore(logicalDevice, semaphore, nullptr);
	}

	void destroyFence(const VkDevice& logicalDevice, const VkFence& fence)
	{
		vkDestroyFence(logicalDevice, fence, nullptr);
	}

	void lockFences(const VkDevice& logicalDevice, const uint32& fenceCount, const VkFence& fence)
	{
		vkWaitForFences(logicalDevice, 1, &fence, VK_TRUE, UINT64_MAX);
	}

	void resetFences(const VkDevice& logicalDevice, const uint32& fenceCount, const VkFence& fence)
	{
		vkResetFences(logicalDevice, fenceCount, &fence);
	}
}