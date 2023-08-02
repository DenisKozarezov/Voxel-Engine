#include "VulkanSwapChainFrame.h"
#include <array>
#include "../vkInit/VulkanInitializers.h"
#include <glm/ext/matrix_transform.hpp>

namespace vkUtils
{
	void SwapChainFrame::makeDescriptorResources(const VkPhysicalDeviceLimits& limits)
	{
		VkDeviceSize size = sizeof(vkUtils::UniformBufferObject);
		uniformBuffers.view = vkUtils::memory::createBuffer(
			physicalDevice,
			logicalDevice,
			size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		std::vector<InstanceData> instanceData;
		instanceData.reserve(INSTANCES_COUNT);
		double cbrt = std::cbrt(INSTANCES_COUNT);
		for (int x = 0; x < cbrt; x++)
		{
			for (int y = 0; y < cbrt; y++)
			{
				for (int z = 0; z < cbrt; z++)
				instanceData.push_back(InstanceData
				{
					.pos = glm::vec3(x, y, z)
				});
			}
		}

		VkDeviceSize instanceBufferSize = sizeof(InstanceData) * instanceData.size();
		uniformBuffers.instances = memory::createBuffer(
			physicalDevice, 
			logicalDevice, 
			instanceBufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

		memory::mapMemory(logicalDevice, uniformBuffers.instances.bufferMemory, 0, instanceBufferSize, 0, instanceData.data());

		uniformBuffers.view.map(logicalDevice);
	}

	void SwapChainFrame::writeDescriptorSet() const
	{
		std::array<VkWriteDescriptorSet, 1> descriptorWrites = {};
		descriptorWrites[0] = vkInit::writeDescriptorSet(
			descriptorSet,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			0,
			&uniformBuffers.view.descriptor);

		vkUpdateDescriptorSets(logicalDevice, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	void SwapChainFrame::release() const
	{
		vkDestroyImageView(logicalDevice, imageView, nullptr);
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
		vkDestroyFence(logicalDevice, inFlightFence, nullptr);
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphore, nullptr);
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphore, nullptr);

		uniformBuffers.view.release(logicalDevice);
		uniformBuffers.instances.release(logicalDevice);
	}
}