//#pragma once
//#include "VulkanBackend.h"
//
//namespace vulkan
//{	
//	const VkDevice& getLogicalDevice();
//
//	/// <summary>
//	/// Returns the physical GPU device currently in use for Vulkan.
//	/// </summary>
//	/// <returns></returns>
//	const VkPhysicalDevice& getPhysicalDevice();
//
//	/// <summary>
//	/// Returns the command buffer used in this frame in flight.
//	/// </summary>
//	/// <returns></returns>
//	const VkCommandBuffer& getCommandBuffer();
//
//	/// <summary>
//	/// Returns a pool of commands for subsequent allocation of command buffers.
//	/// </summary>
//	/// <returns></returns>
//	const VkCommandPool& getCommandPool();
//
//	// ==================== MEMORY ALLOC / DEALLOC ====================
//
//	const uint32 findMemoryType(const uint32& typeFilter, const VkMemoryPropertyFlags& properties);
//	const VkDeviceMemory allocateMemory(const VkMemoryRequirements& requirements, const VkMemoryPropertyFlags& properties);
//	void createBuffer(
//		const VkDeviceSize& size,
//		const VkBufferUsageFlags& usage,
//		const VkMemoryPropertyFlags& properties,
//		VkBuffer& buffer,
//		VkDeviceMemory& bufferMemory);
//	const VkCommandBuffer& beginSingleTimeCommands();
//	void copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const VkDeviceSize& size);
//	void endSingleTimeCommands(const VkCommandBuffer& commandBuffer);
//	void destroyBuffer(const VkBuffer& buffer);
//	void freeDeviceMemory(const VkDeviceMemory& memory);
//	// ===================================================================
//}