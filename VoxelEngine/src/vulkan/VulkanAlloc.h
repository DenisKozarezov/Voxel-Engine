#pragma once
#include <pch.h>
#include "vulkan/VulkanCommandBuffer.h"

namespace vulkan::memory
{
	const uint32 findMemoryType(const VkPhysicalDevice& physicalDevice, const uint32& typeFilter, const VkMemoryPropertyFlags& properties);
	const VkDeviceMemory allocateMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkMemoryRequirements& requirements, const VkMemoryPropertyFlags& properties);
	const VkCommandBuffer& beginSingleTimeCommands();
	void createBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void destroyBuffer(const VkDevice& logicalDevice, const VkBuffer& buffer);
	void freeDeviceMemory(const VkDevice& logicalDevice, const VkDeviceMemory& memory);
}