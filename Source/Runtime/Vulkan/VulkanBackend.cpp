#include "VulkanBackend.h"
#include "vkInit/VulkanInstance.h"
#include "vkInit/VulkanPipeline.h"
#include "vkInit/VulkanFramebuffer.h"
#include "vkInit/VulkanSync.h"
#include "vkInit/VulkanDescriptors.h"
#include "vkInit/VulkanCommand.h"
#include "vkUtils/VulkanMaterials.h"
#include "vkUtils/VulkanStatistics.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <Renderer/UniformBuffer.h>

namespace vulkan
{	
	using namespace components;

	struct VulkanState
	{		
		// Instance-related variables
		const VoxelEngine::Window* window;
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugUtilsMessenger;
		VkDebugReportCallbackEXT debugReportMessenger;
		
		// Device-related variables
		vkInit::VulkanDevice* vulkanDevice = nullptr;
		vkInit::SwapChainBundle swapChainBundle;
		VkSampleCountFlagBits msaaSamples;

		// Command-related variables
		VkCommandPool commandPool;

		// Pipeline-related variables
		VkPipelineLayout pipelineLayout;
		VkPipelineCache pipelineCache;
		VkRenderPass renderPass;
		VkOffset2D viewportPos;
		VkExtent2D viewportSize;
		VkClearColorValue clearColor;

		// Descriptors
		VkDescriptorPool descriptorPool;
		VkDescriptorSetLayout descriptorSetLayout;
		
		bool framebufferResized = false;
	} state;

	renderer::RenderSettings renderSettings;
	renderer::RenderFrameStats renderFrameStats;

	static int MAX_FRAMES_IN_FLIGHT = 3;
	static uint32 CURRENT_FRAME = 0;
		
	VkSurfaceKHR makeInstance()
	{
		state.instance = vkInit::createInstance(VK_API_VERSION_1_2);
		vkUtils::setupDebugReportMessenger(state.instance, &state.debugReportMessenger);
		return vkInit::createSurface(state.instance, state.window);
	}
	void makeDevice(const VkSurfaceKHR& surface)
	{
		state.vulkanDevice = new vkInit::VulkanDevice(state.instance, surface);
		state.msaaSamples = vkInit::findMaxSamplesCount(state.vulkanDevice->limits);
		RUNTIME_TRACE("Device max samples count: {0}.", (int)state.msaaSamples);
	}
	void makeSwapChain()
	{
		state.swapChainBundle = vkInit::createSwapChain(
			state.vulkanDevice,
			state.window->getWidth(), 
			state.window->getHeight(), 
			state.msaaSamples);

		MAX_FRAMES_IN_FLIGHT = static_cast<int>(state.swapChainBundle.frames.size());

		for (vkUtils::SwapChainFrame& frame : state.swapChainBundle.frames)
		{
			frame.vulkanDevice = state.vulkanDevice;
		}
	}
	void makeFramebuffers()
	{
		const VkExtent2D swapChainExtent = state.swapChainBundle.extent;
		const VkImageView colorView = state.swapChainBundle.colorImageView;
		const VkImageView depthImageView = state.swapChainBundle.depthImageView;
		auto& frames = state.swapChainBundle.frames;

		for (auto& frame : frames)
		{
			std::vector<VkImageView> attachments = { colorView, depthImageView, frame.imageView };
			frame.framebuffer = vkInit::createFramebuffer(state.vulkanDevice->logicalDevice, state.renderPass, swapChainExtent, attachments);
		}
	}
	void makeDescriptorSetLayout()
	{
		vkInit::DescriptorSetLayoutInputBundle bindings;
		bindings.count = 1;

		bindings.indices.push_back(0);
		bindings.types.push_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		bindings.stages.push_back(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT);
		bindings.counts.push_back(1);

	/*	bindings.indices.push_back(0);
		bindings.types.push_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		bindings.stages.push_back(VK_SHADER_STAGE_FRAGMENT_BIT);
		bindings.counts.push_back(1);*/

		state.descriptorSetLayout = vkInit::createDescriptorSetLayout(state.vulkanDevice->logicalDevice, bindings);
	}
	void makeGraphicsPipeline()
	{
		VkFormat swapChainImageFormat = state.swapChainBundle.format;
		VkFormat depthFormat = state.swapChainBundle.depthFormat;
		state.renderPass = vkInit::createRenderPass(state.vulkanDevice->logicalDevice, swapChainImageFormat, depthFormat, state.msaaSamples);
		
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = vkInit::pipelineInputAssemblyStateCreateInfo(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_FALSE);
		
		VkPipelineViewportStateCreateInfo viewportState = vkInit::pipelineViewportStateCreateInfo(1, 1);

		VkPipelineRasterizationStateCreateInfo rasterizer = vkInit::pipelineRasterizationStateCreateInfo(
			VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_BACK_BIT,
			VK_FRONT_FACE_COUNTER_CLOCKWISE);

		VkPipelineMultisampleStateCreateInfo multisampling = vkInit::pipelineMultisampleStateCreateInfo(state.msaaSamples);

		VkPipelineColorBlendAttachmentState colorBlendAttachment = vkInit::pipelineColorBlendAttachmentState(
			VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT,
			VK_FALSE);
		VkPipelineColorBlendStateCreateInfo colorBlending = vkInit::pipelineColorBlendStateCreateInfo(colorBlendAttachment);

		VkPipelineDepthStencilStateCreateInfo depthStencil = vkInit::pipelineDepthStencilStateCreateInfo(
			VK_TRUE,
			VK_TRUE,
			VK_COMPARE_OP_LESS_OR_EQUAL);

		std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
			VK_DYNAMIC_STATE_LINE_WIDTH
		};
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = vkInit::pipelineDynamicStateCreateInfo(dynamicStates);

		VkPipelineLayoutCreateInfo layoutCreateInfo = vkInit::pipelineLayoutCreateInfo(&state.descriptorSetLayout);
		
		auto pipelineBuilder = vkInit::VulkanGraphicsPipelineBuilder(state.vulkanDevice->logicalDevice, state.pipelineCache)
			.setInputAssemblyState(&inputAssembly)
			.setViewportState(&viewportState)
			.setRasterizationState(&rasterizer)
			.setMultisampleState(&multisampling)
			.setColorBlendState(&colorBlending)
			.setColorBlendAttachment(&colorBlendAttachment)
			.setDepthStencilState(&depthStencil)
			.setDynamicState(&dynamicStateCreateInfo)
			.setLayout(&layoutCreateInfo)
			.setRenderPass(state.renderPass);
		
		vkUtils::makeMaterials(state.vulkanDevice->logicalDevice, pipelineBuilder);
	}
	void makeFrameResources()
	{
		const vkInit::VulkanDevice* device = state.vulkanDevice;

		int i = 0;
		for (vkUtils::SwapChainFrame& frame : state.swapChainBundle.frames)
		{
			frame.imageAvailableSemaphore = vkInit::createSemaphore(device->logicalDevice);
			frame.renderFinishedSemaphore = vkInit::createSemaphore(device->logicalDevice);
			frame.inFlightFence = vkInit::createFence(device->logicalDevice);
			frame.descriptorSet = vkInit::allocateDescriptorSet(device->logicalDevice, state.descriptorPool, state.descriptorSetLayout);

			frame.makeDescriptorResources(device->limits);

			RUNTIME_TRACE("Vulkan frame resources created for frame {0}.", i);
			++i;
		}
	}	
	void makeCommandBuffers()
	{
		int i = 0;
		for (vkUtils::SwapChainFrame& frame : state.swapChainBundle.frames)
		{
			frame.commandBuffer = vkUtils::memory::allocateCommandBuffer(state.commandPool);
			
			RUNTIME_TRACE("Vulkan command buffer allocated for frame {0}.", i);
			++i;
		}
	}
	void finalizeSetup()
	{
		makeFramebuffers();

		state.commandPool = vkInit::createCommandPool(state.vulkanDevice->logicalDevice, state.vulkanDevice->queueFamilyIndices.graphicsFamily.value());

		makeCommandBuffers();

		state.descriptorPool = vkInit::createDescriptorPool(state.vulkanDevice->logicalDevice);

		makeFrameResources();
	}
	
	void recreateSwapChain()
	{
		uint16 width = state.window->getWidth();
		uint16 height = state.window->getHeight();

		while (width == 0 || height == 0)
		{
			width = state.window->getWidth();
			height = state.window->getHeight();
			state.window->waitEvents();
		}
		deviceWaitIdle();

		cleanupSwapChain();

		makeSwapChain();
		makeFramebuffers();
		makeFrameResources();
		makeCommandBuffers();
	}	
	void recordCommandBuffer(const VkCommandBuffer& commandBuffer)
	{
		// =================== RENDER WHOLE STUFF HERE ! ===================		
		ImDrawData* main_draw_data = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(main_draw_data, commandBuffer);
		
		vkCmdEndRenderPass(commandBuffer);
		vkUtils::memory::endCommand(commandBuffer);
	}
	void submitToQueue(const VkQueue& queue, const VkCommandBuffer& commandBuffer, const VkSemaphore* signalSemaphores)
	{
		vkUtils::SwapChainFrame& frame = state.swapChainBundle.frames[CURRENT_FRAME];

		VkSemaphore waitSemaphores[] = { frame.imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo = vkInit::submitInfo(
			waitSemaphores, 
			signalSemaphores, 
			commandBuffer, 
			waitStages);	

		VkResult err = vkQueueSubmit(queue, 1, &submitInfo, frame.inFlightFence);
		VK_CHECK(err, "failed to submit draw command buffer!");
	}
	void cleanupSwapChain()
	{
		state.swapChainBundle.release(*state.vulkanDevice);	

		for (vkUtils::SwapChainFrame& frame : state.swapChainBundle.frames)
		{
			vkUtils::memory::releaseCommandBuffer(state.vulkanDevice->logicalDevice, frame.commandBuffer, state.commandPool);
		}
	}
	void presentFrame(const uint32& imageIndex, VkSemaphore* signalSemaphores)
	{
		VkSwapchainKHR swapChains[] = { state.swapChainBundle.swapchain };

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		VkResult err = vkQueuePresentKHR(state.vulkanDevice->deviceQueues.presentQueue, &presentInfo);

		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR || state.framebufferResized)
		{
			state.framebufferResized = false;
			recreateSwapChain();
		}
		else VK_CHECK(err, "failed to present swap chain image!");
	}
	void prepareFrame()
	{
		const vkUtils::SwapChainFrame& frame = state.swapChainBundle.frames[CURRENT_FRAME];

		frame.writeDescriptorSet();

		const VkCommandBuffer commandBuffer = frame.commandBuffer;
		const VkExtent2D swapChainExtent = state.swapChainBundle.extent;

		std::vector<VkClearValue> clearValues(2);
		clearValues[0].color = state.clearColor;
		clearValues[1].depthStencil = { 1.0f, 0 };

		vkUtils::memory::beginCommand(commandBuffer);
				
		const VkRenderPassBeginInfo renderPassInfo = vkInit::renderPassBeginInfo(
			state.renderPass,
			frame.framebuffer,
			swapChainExtent,
			clearValues);
		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = vkInit::viewport(state.viewportSize, 0.0f, 1.0f);
		viewport.x = static_cast<float>(state.viewportPos.x);
		viewport.y = static_cast<float>(state.viewportPos.y);
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor = vkInit::rect2D(swapChainExtent, { 0, 0 });
		scissor.offset.x = state.viewportPos.x;
		scissor.offset.y = state.viewportPos.y;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
		
		state.vulkanDevice->queryPool->beginQuery(commandBuffer);
	}
	void beginFrame(const UniformBufferObject& ubo)
	{
		const vkUtils::SwapChainFrame& frame = state.swapChainBundle.frames[CURRENT_FRAME];

		// Stage 1. ACQUIRE IMAGE FROM SWAPCHAIN
		vkInit::lockFences(state.vulkanDevice->logicalDevice, &frame.inFlightFence);

		uint32 imageIndex;
		VkResult result = vkAcquireNextImageKHR(state.vulkanDevice->logicalDevice, state.swapChainBundle.swapchain, UINT64_MAX, frame.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return;
		}
		else
		{
			RUNTIME_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "failed to acquire swap chain image!");
		}

		vkInit::resetFences(state.vulkanDevice->logicalDevice, &frame.inFlightFence);
		vkUtils::memory::resetCommandBuffer(frame.commandBuffer);

		frame.uniformBuffers.view.setData(&ubo, sizeof(ubo));

		RaymarchData data{};
		data.resolution = glm::vec2(state.viewportSize.width, state.viewportSize.height);
		data.mousePos = glm::vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
		data.voxelSize = 0.5f;
		frame.uniformBuffers.raymarch.setData(&data, sizeof(data));

		prepareFrame();
	}
	void endFrame()
	{
		const vkUtils::SwapChainFrame& frame = state.swapChainBundle.frames[CURRENT_FRAME];

		// Stage 2. GRAPHICS
		state.vulkanDevice->queryPool->endQuery(frame.commandBuffer);
		recordCommandBuffer(frame.commandBuffer);

		VkSemaphore signalSemaphores[] = { frame.renderFinishedSemaphore};
		submitToQueue(state.vulkanDevice->deviceQueues.graphicsQueue, frame.commandBuffer, signalSemaphores);

		state.vulkanDevice->queryPool->getQueryResults();
		
		// Stage 3. PRESENT
		presentFrame(CURRENT_FRAME, signalSemaphores);
		
		CURRENT_FRAME = (CURRENT_FRAME + 1) % MAX_FRAMES_IN_FLIGHT;		
	}

	void resize(const uint32& width, const uint32& height)
	{
		state.framebufferResized = true;
	}
	void setClearColor(const glm::vec4 color)
	{
		state.clearColor = { {color[0], color[1], color[2], color[3] }};
	}
	void setViewport(const int32_t& x, const int32_t& y, const uint32& width, const uint32& height)
	{
		state.viewportPos = VkOffset2D(x, y);

		if (width > 0 && height > 0)
			state.viewportSize = VkExtent2D(width, height);
	}
	void init(const Window& window)
	{
		state.window = &window;
		
		const VkSurfaceKHR surface = makeInstance();

		makeDevice(surface);

		makeSwapChain();

		makeDescriptorSetLayout();
		
		makeGraphicsPipeline();
		
		finalizeSetup();

		initImGui();

		RUNTIME_TRACE("Vulkan setup ended.");
	}
	void initImGui()
	{
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = state.instance;
		init_info.PhysicalDevice = state.vulkanDevice->physicalDevice;
		init_info.Device = state.vulkanDevice->logicalDevice;
		init_info.QueueFamily = state.vulkanDevice->queueFamilyIndices.graphicsFamily.value();
		init_info.Queue = state.vulkanDevice->deviceQueues.graphicsQueue;
		init_info.PipelineCache = state.pipelineCache;
		init_info.DescriptorPool = state.descriptorPool;
		init_info.Subpass = 0;
		init_info.MinImageCount = MAX_FRAMES_IN_FLIGHT;
		init_info.ImageCount = MAX_FRAMES_IN_FLIGHT;
		init_info.MSAASamples = state.msaaSamples;
		init_info.Allocator = nullptr;
		ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)state.window->getNativeWindow(), true);
		ImGui_ImplVulkan_Init(&init_info, state.renderPass);

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		const string fontPath = Paths::assetsDir() + "fonts/Roboto-Medium.ttf";

		io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 14.0f);
		{
			const VkCommandBuffer commandBuffer = vkUtils::memory::beginSingleTimeCommands(state.commandPool);
			ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
			endSingleTimeCommands(commandBuffer);
			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}
	}
	void deviceWaitIdle()
	{
		vkDeviceWaitIdle(state.vulkanDevice->logicalDevice);
	}
	void cleanup()
	{
		cleanupSwapChain();

		vkUtils::releaseMaterials();

		vkDestroyPipelineLayout(state.vulkanDevice->logicalDevice, state.pipelineLayout, nullptr);
		vkDestroyPipelineCache(state.vulkanDevice->logicalDevice, state.pipelineCache, nullptr);
		vkDestroyRenderPass(state.vulkanDevice->logicalDevice, state.renderPass, nullptr);
		vkDestroyDescriptorPool(state.vulkanDevice->logicalDevice, state.descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(state.vulkanDevice->logicalDevice, state.descriptorSetLayout, nullptr);

		vkDestroyCommandPool(state.vulkanDevice->logicalDevice, state.commandPool, nullptr);
		vkDestroySurfaceKHR(state.instance, state.vulkanDevice->surface, nullptr);

		delete state.vulkanDevice;

		if (vkUtils::_enableValidationLayers)
		{
			vkUtils::destroyDebugReportMessengerEXT(state.instance, state.debugReportMessenger, nullptr);
		}

		vkDestroyInstance(state.instance, nullptr);
	}

	renderer::RenderSettings& getRenderSettings()
	{
		return renderSettings;
	}
	const renderer::RenderFrameStats& getFrameStats()
	{
		return renderFrameStats;
	}

	const renderer::ShaderStats& getShaderStats()
	{
		return state.vulkanDevice->queryPool->getStats();
	}

	void resetFrameStats()
	{
		renderFrameStats.drawCalls = 0;
	}

	const vkInit::VulkanDevice* getDevice()
	{
		return state.vulkanDevice;
	}
	const VkCommandBuffer& getCommandBuffer()
	{
		return state.swapChainBundle.frames[CURRENT_FRAME].commandBuffer;
	}
	const vkUtils::SwapChainFrame& getCurrentFrame()
	{
		return state.swapChainBundle.frames[CURRENT_FRAME];
	}
	const VkCommandPool& getCommandPool()
	{
		return state.commandPool;
	}

	// ==================== MEMORY ALLOC / DEALLOC ====================
	void copyBuffer(const vkUtils::memory::Buffer& srcBuffer, vkUtils::memory::Buffer& dstBuffer, const VkDeviceSize& size)
	{
		const VkCommandBuffer commandBuffer = vkUtils::memory::beginSingleTimeCommands(state.commandPool);

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		dstBuffer.size = size;

		endSingleTimeCommands(commandBuffer);
	}
	void endSingleTimeCommands(const VkCommandBuffer& commandBuffer)
	{
		vkUtils::memory::endCommand(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(state.vulkanDevice->deviceQueues.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(state.vulkanDevice->deviceQueues.graphicsQueue);

		vkUtils::memory::releaseCommandBuffer(state.vulkanDevice->logicalDevice, commandBuffer, state.commandPool);
	}
	
	void copyBufferToImage(vkUtils::memory::Buffer buffer, VkImage image, uint32 width, uint32 height) 
	{
		VkCommandBuffer commandBuffer = vkUtils::memory::beginSingleTimeCommands(state.commandPool);

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		endSingleTimeCommands(commandBuffer);
	}
	void copyImage(VkCommandPool cmdPool, VkImage srcImageId, VkImage dstImageId, uint32 width, uint32 height)
	{
		VkCommandBuffer cmdBuffer = vkUtils::memory::beginSingleTimeCommands(cmdPool);

		VkImageSubresourceLayers subResource = {};
		subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subResource.baseArrayLayer = 0;
		subResource.layerCount = 1;
		subResource.mipLevel = 0;

		VkImageCopy region{};
		region.srcOffset = { 0, 0, 0 };
		region.srcSubresource = subResource;
		region.dstOffset = { 0, 0, 0 };
		region.dstSubresource = subResource;
		region.extent.width = width;
		region.extent.height = height;
		region.extent.depth = 1;

		vkCmdCopyImage(
			cmdBuffer,
			srcImageId, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			dstImageId, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &region);

		vkUtils::memory::endCommand(cmdBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffer;

		vkQueueSubmit(state.vulkanDevice->deviceQueues.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(state.vulkanDevice->deviceQueues.graphicsQueue);

		vkUtils::memory::releaseCommandBuffer(state.vulkanDevice->logicalDevice, cmdBuffer, cmdPool);
	}
	void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) 
	{
		const VkCommandBuffer commandBuffer = vkUtils::memory::beginSingleTimeCommands(state.commandPool);

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
			throw std::invalid_argument("unsupported layout transition!");

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		endSingleTimeCommands(commandBuffer);
	}
	// =============================================================
}
