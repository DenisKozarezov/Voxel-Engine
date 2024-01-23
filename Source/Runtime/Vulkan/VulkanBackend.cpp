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
#include <Renderer/RenderingStructs.h>

namespace vulkan
{	
	using namespace components;

	struct VulkanState
	{		
		// Instance-related variables
		const Window* window;
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
	static uint32 IMAGE_INDEX = 0;
		
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
		bindings.stages.push_back(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT);
		bindings.counts.push_back(1);
		
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
		state.vulkanDevice->queryPool->endQuery(commandBuffer);
		
		// =================== RENDER WHOLE STUFF HERE ! ===================		
		ImDrawData* main_draw_data = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(main_draw_data, commandBuffer);
		
		vkCmdEndRenderPass(commandBuffer);
		vkUtils::memory::endCommand(commandBuffer);
	}
	void submitToQueue(const vkUtils::SwapChainFrame& frame, const VkQueue& queue)
	{
		const VkSemaphore waitSemaphores[] = { frame.imageAvailableSemaphore };
		constexpr VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		const VkSubmitInfo submitInfo = vkInit::submitInfo(
			waitSemaphores, 
			&frame.renderFinishedSemaphore, 
			frame.commandBuffer, 
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
	void presentFrame(const vkUtils::SwapChainFrame& frame)
	{
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &frame.renderFinishedSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &state.swapChainBundle.swapchain;
		presentInfo.pImageIndices = &IMAGE_INDEX;
		presentInfo.pNext = nullptr;

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
		const VkFramebuffer framebuffer = state.swapChainBundle.frames[IMAGE_INDEX].framebuffer;

		frame.writeDescriptorSet();
		
		vkUtils::memory::beginCommand(frame.commandBuffer);
		
		state.vulkanDevice->queryPool->resetQuery(frame.commandBuffer);

		const VkExtent2D swapChainExtent = state.swapChainBundle.extent;

		std::vector<VkClearValue> clearValues(2);
		clearValues[0].color = state.clearColor;
		clearValues[1].depthStencil = { 1.0f, 0 };
				
		const VkRenderPassBeginInfo renderPassInfo = vkInit::renderPassBeginInfo(
			state.renderPass,
			framebuffer,
			swapChainExtent,
			clearValues);
		vkCmdBeginRenderPass(frame.commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = vkInit::viewport(state.viewportSize, 0.0f, 1.0f);
		viewport.x = static_cast<float>(state.viewportPos.x);
		viewport.y = static_cast<float>(state.viewportPos.y);
		vkCmdSetViewport(frame.commandBuffer, 0, 1, &viewport);

		VkRect2D scissor = vkInit::rect2D(swapChainExtent, { 0, 0 });
		scissor.offset.x = state.viewportPos.x;
		scissor.offset.y = state.viewportPos.y;
		vkCmdSetScissor(frame.commandBuffer, 0, 1, &scissor);

		state.vulkanDevice->queryPool->beginQuery(frame.commandBuffer);
	}
	void beginFrame(const UniformBufferObject& ubo)
	{
		const vkUtils::SwapChainFrame& frame = state.swapChainBundle.frames[CURRENT_FRAME];

		// Stage 1. ACQUIRE IMAGE FROM SWAPCHAIN
		vkInit::lockFences(state.vulkanDevice->logicalDevice, &frame.inFlightFence);
		
		VkResult result = vkAcquireNextImageKHR(state.vulkanDevice->logicalDevice, state.swapChainBundle.swapchain, UINT64_MAX, frame.imageAvailableSemaphore, VK_NULL_HANDLE, &IMAGE_INDEX);
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

		prepareFrame();
	}
	void endFrame()
	{
		const vkUtils::SwapChainFrame& frame = state.swapChainBundle.frames[CURRENT_FRAME];

		// Stage 2. GRAPHICS
		recordCommandBuffer(frame.commandBuffer);		
		submitToQueue(frame, state.vulkanDevice->deviceQueues.graphicsQueue);

		state.vulkanDevice->queryPool->getQueryResults();
		
		// Stage 3. PRESENT
		presentFrame(frame);
		
		CURRENT_FRAME = (CURRENT_FRAME + 1) % MAX_FRAMES_IN_FLIGHT;		
	}

	void resize(const uint16& width, const uint16& height)
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
			vkUtils::memory::endSingleTimeCommands(
				state.vulkanDevice->logicalDevice,
				state.commandPool,
				state.vulkanDevice->deviceQueues.graphicsQueue,
				commandBuffer);
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

#ifdef ENABLE_VALIDATION_LAYERS
		vkUtils::destroyDebugReportMessengerEXT(state.instance, state.debugReportMessenger, nullptr);
#endif
		
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

	// ==================== MEMORY ALLOC / DEALLOC ====================
	void copyBuffer(const vkUtils::memory::Buffer& srcBuffer, vkUtils::memory::Buffer& dstBuffer, const VkDeviceSize& size)
	{
		const VkCommandBuffer commandBuffer = vkUtils::memory::beginSingleTimeCommands(state.commandPool);

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		dstBuffer.size = size;

		vkUtils::memory::endSingleTimeCommands(
			state.vulkanDevice->logicalDevice,
			state.commandPool,
			state.vulkanDevice->deviceQueues.graphicsQueue,
			commandBuffer);
	}
	// =============================================================
}
