//#include "VulkanFrontend.h"
//
//namespace vulkan
//{
//	struct VulkanState
//	{
//		GLFWwindow* window;
//		VkInstance instance;
//		VkDebugUtilsMessengerEXT debugUtilsMessenger;
//		VkDebugReportCallbackEXT debugReportMessenger;
//		VkDevice logicalDevice;
//		VkPhysicalDevice physicalDevice;
//		VkCommandPool commandPool;
//		VkQueue graphicsQueue;
//		VkQueue presentQueue;
//		VkPipelineCache pipelineCache;
//		QueueFamilyIndices queueFamilyIndices;
//		VkSurfaceKHR surface;
//		VkSwapchainKHR swapChain;
//		VkRenderPass renderPass;
//		VkDescriptorPool descriptorPool;
//		VkPipelineLayout pipelineLayout;
//		VkDescriptorSetLayout descriptorSetLayout;
//		VkPipeline graphicsPipeline;
//		VkFormat swapChainImageFormat;
//		VkExtent2D swapChainExtent;
//		std::vector<VkImage> swapChainImages;
//		std::vector<VkImageView> swapChainImageViews;
//		std::vector<Framebuffer> swapChainFramebuffers;
//		std::vector<VkCommandBuffer> commandBuffers;
//		std::vector<VkSemaphore> imageAvailableSemaphores;
//		std::vector<VkSemaphore> renderFinishedSemaphores;
//		std::vector<VkFence> inFlightFences;
//		bool framebufferResized = false;
//	} state;
//
//	const VkDevice& getLogicalDevice()
//	{
//		return state.logicalDevice;
//	}
//	const VkPhysicalDevice& getPhysicalDevice()
//	{
//		return state.physicalDevice;
//	}
//	const VkCommandBuffer& getCommandBuffer()
//	{
//		return state.commandBuffers[CURRENT_FRAME];
//	}
//	const VkCommandPool& getCommandPool()
//	{
//		return state.commandPool;
//	}
//
//	// ==================== MEMORY ALLOC / DEALLOC ====================
//	const uint32 findMemoryType(const uint32& typeFilter, const VkMemoryPropertyFlags& properties)
//	{
//		return memory::findMemoryType(state.physicalDevice, typeFilter, properties);
//	}
//	const VkDeviceMemory allocateMemory(const VkMemoryRequirements& requirements, const VkMemoryPropertyFlags& properties)
//	{
//		return memory::allocateMemory(state.physicalDevice, state.logicalDevice, requirements, properties);
//	}
//	const VkCommandBuffer& beginSingleTimeCommands()
//	{
//		return memory::beginSingleTimeCommands();
//	}
//	void createBuffer(
//		const VkDeviceSize& size,
//		const VkBufferUsageFlags& usage,
//		const VkMemoryPropertyFlags& properties,
//		VkBuffer& buffer,
//		VkDeviceMemory& bufferMemory)
//	{
//		memory::createBuffer(
//			state.physicalDevice,
//			state.logicalDevice,
//			size,
//			usage,
//			properties,
//			buffer,
//			bufferMemory);
//	}
//	void destroyBuffer(const VkBuffer& buffer)
//	{
//		memory::destroyBuffer(state.logicalDevice, buffer);
//	}
//	void freeDeviceMemory(const VkDeviceMemory& memory)
//	{
//		memory::freeDeviceMemory(state.logicalDevice, memory);
//	}
//	void endSingleTimeCommands(const VkCommandBuffer& commandBuffer)
//	{
//		memory::CommandBuffer::endCommand(commandBuffer);
//
//		VkSubmitInfo submitInfo = {};
//		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//		submitInfo.commandBufferCount = 1;
//		submitInfo.pCommandBuffers = &commandBuffer;
//
//		vkQueueSubmit(state.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
//		vkQueueWaitIdle(state.graphicsQueue);
//
//		memory::CommandBuffer::release(commandBuffer);
//	}
//	void copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const VkDeviceSize& size)
//	{
//		VkCommandBuffer commandBuffer = beginSingleTimeCommands();
//
//		VkBufferCopy copyRegion = {};
//		copyRegion.size = size;
//		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
//
//		endSingleTimeCommands(commandBuffer);
//	}
//	// =============================================================
//}