#include "VulkanSwapChainFrame.h"
#include <array>
#include "../vkInit/VulkanInitializers.h"

namespace vkUtils
{
	void SwapChainFrame::makeDescriptorResources(const VkPhysicalDeviceLimits& limits)
	{
		// Static shared uniform buffer object with projection and view matrix
		VkDeviceSize size = sizeof(vkUtils::UniformBufferObject);
		uniformBuffers.view = vkUtils::memory::createBuffer(
			physicalDevice,
			logicalDevice,
			size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		// Dynamic uniform buffer object with per-object matrices
		size_t minUboAlignment = limits.minUniformBufferOffsetAlignment;
		dynamicAlignment = sizeof(glm::mat4);
		if (minUboAlignment > 0) 
		{
			dynamicAlignment = memory::alignedSize(dynamicAlignment, minUboAlignment);
		}
		
		size = dynamicAlignment * 2;
		uniformBuffers.dynamic = vkUtils::memory::createBuffer(
			physicalDevice,
			logicalDevice,
			size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		uniformBuffers.dynamic.descriptor.range = dynamicAlignment;
		
		uboDataDynamic.model = (glm::mat4*)memory::alignedAlloc(size, dynamicAlignment);

		uniformBuffers.view.map(logicalDevice);
		uniformBuffers.dynamic.map(logicalDevice);
	}

	void SwapChainFrame::writeDescriptorSet() const
	{
		std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};
		descriptorWrites[0] = vkInit::writeDescriptorSet(
			descriptorSet,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			0,
			&uniformBuffers.view.descriptor);

		descriptorWrites[1] = vkInit::writeDescriptorSet(
			descriptorSet,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
			1,
			&uniformBuffers.dynamic.descriptor);

		// Flush to make changes visible to the host
		VkMappedMemoryRange memoryRange = vkInit::mappedMemoryRange();
		memoryRange.memory = uniformBuffers.dynamic.bufferMemory;
		memoryRange.size = uniformBuffers.dynamic.size;
		vkFlushMappedMemoryRanges(logicalDevice, 1, &memoryRange);

		vkUpdateDescriptorSets(logicalDevice, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	void SwapChainFrame::release() const
	{
		vkDestroyImageView(logicalDevice, imageView, nullptr);
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
		vkDestroyFence(logicalDevice, inFlightFence, nullptr);
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphore, nullptr);
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphore, nullptr);

		if (uboDataDynamic.model) 
		{
			memory::alignedFree(uboDataDynamic.model);
		}

		uniformBuffers.view.release(logicalDevice);
		uniformBuffers.dynamic.release(logicalDevice);
	}
}