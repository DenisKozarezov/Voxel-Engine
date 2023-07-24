#define GLFW_EXPOSE_NATIVE_WIN32
#include "VulkanBackend.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include "VulkanDevice.h"
#include "VulkanFramebuffer.h"
#include "VulkanShader.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSync.h"
#include "assets_management/AssetsProvider.h"

namespace vulkan
{	
	struct VulkanState
	{
		GLFWwindow* windowPtr;
		const VoxelEngine::Window* window;
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugUtilsMessenger;
		VkDebugReportCallbackEXT debugReportMessenger;
		VkDevice logicalDevice;
		VkPhysicalDevice physicalDevice;
		VkCommandPool commandPool;
		DeviceQueues queues;
		VkPipelineCache pipelineCache;
		QueueFamilyIndices queueFamilyIndices;
		VkSurfaceKHR surface;
		VkSwapchainKHR swapChain;
		VkRenderPass renderPass;
		VkDescriptorPool descriptorPool;
		VkPipelineLayout pipelineLayout;
		VkDescriptorSetLayout descriptorSetLayout;
		VkPipeline graphicsPipeline;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		std::vector<SwapChainFrame> swapChainFrames;
		bool framebufferResized = false;
	} state;

	VoxelEngine::components::camera::Camera* FPVcamera;
		
	void framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		state.framebufferResized = true;
	}
	
	constexpr bool hasStencilComponent(const VkFormat& format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}	
	const std::vector<const char*> getRequiredExtensions()
	{
		uint32 glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
		if (_enableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

			if (extensions.size() > 0)
			{
				std::stringstream ss;
				for (const char* extensionName : extensions)
				{
					ss << '\t';
					ss << extensionName;
					ss << '\n';
				}
				VOXEL_CORE_TRACE("Device extensions to be requested:\n" + ss.str())
			}
		}
		return extensions;
	}
	constexpr const VkFormat findSupportedFormat(const VkPhysicalDevice& physicalDevice, const std::vector<VkFormat>& candidates, const VkImageTiling& tiling, const VkFormatFeatureFlags& features)
	{	
		for (const VkFormat& format : candidates) 
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			const bool isTilingLinear = tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features;
			const bool isTilingOptimal = tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features;

			if (isTilingLinear) return format;
			else if (isTilingOptimal) return format;
		}
		throw std::runtime_error("failed to find supported format!");
	}
	constexpr const VkFormat& findDepthFormat(const VkPhysicalDevice& physicalDevice)
	{
		return findSupportedFormat(physicalDevice,
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}	
	void createInstance()
	{
		bool layersSupported = _enableValidationLayers && checkValidationLayerSupport();
		VOXEL_CORE_ASSERT(layersSupported, "validation layers requested, but not available!")

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Voxel Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (_enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();			
			VkDebugReportCallbackCreateInfoEXT debugCreateInfo = {};
			debugCreateInfo = populateDebugReportCreateInfo();
			createInfo.pNext = (VkDebugReportCallbackCreateInfoEXT*)&debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		VkResult err = vkCreateInstance(&createInfo, nullptr, &state.instance);
		check_vk_result(err, "failed to create instance!");

		VOXEL_CORE_TRACE("Vulkan instance created.")
	}
	
	const VkSurfaceKHR createSurface(const VkInstance& instance, GLFWwindow* window)
	{
		VkSurfaceKHR surface;
		VkResult err = glfwCreateWindowSurface(instance, window, nullptr, &surface);
		check_vk_result(err, "failed to create window surface!");

		VOXEL_CORE_TRACE("Vulkan surface created.")

		return surface;
	}
	void createRenderPass(const VkDevice& logicalDevice, const VkFormat& swapChainImageFormat)
	{
		VkAttachmentDescription colorAttachment = vulkan::initializers::renderPassColorAttachment(swapChainImageFormat);

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		std::vector<VkAttachmentDescription> attachments = { colorAttachment };
		std::vector<VkSubpassDescription> subpasses = { subpass };
		std::vector<VkSubpassDependency> dependencies = { dependency };
		VkRenderPassCreateInfo renderPassInfo = vulkan::initializers::renderPassCreateInfo(attachments, subpasses, dependencies);

		VkResult err = vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &state.renderPass);
		check_vk_result(err, "failed to create render pass!");
	}
	void createGraphicsPipeline(
		const VkDevice& logicalDevice, 
		const VkRenderPass& renderPass, 
		const VkDescriptorSetLayout& descriptorSetLayout)
	{
		shaders::VulkanShader vertexShader = shaders::VulkanShader(state.logicalDevice, ASSET_PATH("shaders/vert.spv"), VK_SHADER_STAGE_VERTEX_BIT);
		shaders::VulkanShader fragShader = shaders::VulkanShader(state.logicalDevice, ASSET_PATH("shaders/frag.spv"), VK_SHADER_STAGE_FRAGMENT_BIT);

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { vertexShader.getStage(), fragShader.getStage() };
		const auto& bindingDescription = Vertex::getBindingDescription();
		const auto& attributeDescription = Vertex::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = vulkan::initializers::pipelineVertexInputStateCreateInfo(
			&bindingDescription,
			1,
			attributeDescription.data(),
			static_cast<uint32>(attributeDescription.size()));

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = vulkan::initializers::pipelineInputAssemblyStateCreateInfo(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			VK_FALSE);

		VkPipelineViewportStateCreateInfo viewportState = vulkan::initializers::pipelineViewportStateCreateInfo(1, 1);

		VkPipelineRasterizationStateCreateInfo rasterizer = vulkan::initializers::pipelineRasterizationStateCreateInfo(
			VK_POLYGON_MODE_FILL, 
			VK_CULL_MODE_BACK_BIT, 
			VK_FRONT_FACE_COUNTER_CLOCKWISE);

		VkPipelineMultisampleStateCreateInfo multisampling = vulkan::initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);

		VkPipelineColorBlendAttachmentState colorBlendAttachment = vulkan::initializers::pipelineColorBlendAttachmentState(
			VK_COLOR_COMPONENT_R_BIT | 
			VK_COLOR_COMPONENT_G_BIT | 
			VK_COLOR_COMPONENT_B_BIT | 
			VK_COLOR_COMPONENT_A_BIT, 
			VK_FALSE);
		VkPipelineColorBlendStateCreateInfo colorBlending = vulkan::initializers::pipelineColorBlendStateCreateInfo(colorBlendAttachment);

		std::vector<VkDynamicState> dynamicStates = 
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = vulkan::initializers::pipelineDynamicStateCreateInfo(dynamicStates);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = vulkan::initializers::pipelineLayoutCreateInfo(&descriptorSetLayout);

		VkResult err = vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &state.pipelineLayout);
		check_vk_result(err, "failed to create pipeline layout!");

		VkGraphicsPipelineCreateInfo pipelineInfo = vulkan::initializers::pipelineCreateInfo();
		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineInfo.layout = state.pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		err = vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &state.graphicsPipeline);
		check_vk_result(err, "failed to create graphics pipeline!");

		vertexShader.unbind();
		fragShader.unbind();

		VOXEL_CORE_TRACE("Vulkan graphics pipeline created.")
	}
	void createCommandPool(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice)
	{
		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice, state.surface);

		VkCommandPoolCreateInfo poolInfo = vulkan::initializers::commandPoolCreateInfo(queueFamilyIndices.graphicsFamily.value(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		VkResult err = vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &state.commandPool);
		check_vk_result(err, "failed to create command pool!");
	}
	void createCommandBuffers()
	{
		for (auto& frame : state.swapChainFrames)
		{
			frame.commandBuffer = memory::CommandBuffer::allocate();
		}
	}
	void createSyncObjects(const VkDevice& logicalDevice)
	{
		for (SwapChainFrame& frame : state.swapChainFrames) 
		{
			frame.imageAvailableSemaphore = createSemaphore(logicalDevice);
			frame.renderFinishedSemaphore = createSemaphore(logicalDevice);
			frame.inFlightFence = createFence(logicalDevice);
		}
	}	
	void initImGui()
	{
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = state.instance;
		init_info.PhysicalDevice = state.physicalDevice;
		init_info.Device = state.logicalDevice;
		init_info.QueueFamily = state.queueFamilyIndices.graphicsFamily.value();
		init_info.Queue = state.queues.graphicsQueue;
		init_info.PipelineCache = state.pipelineCache;
		init_info.DescriptorPool = state.descriptorPool;
		init_info.Subpass = 0;
		init_info.MinImageCount = MAX_FRAMES_IN_FLIGHT;
		init_info.ImageCount = MAX_FRAMES_IN_FLIGHT;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = nullptr;
		ImGui_ImplGlfw_InitForVulkan(state.windowPtr, true);
		ImGui_ImplVulkan_Init(&init_info, state.renderPass);
		
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.Fonts->AddFontDefault();
		{
			VkCommandBuffer commandBuffer = state.swapChainFrames[CURRENT_FRAME].commandBuffer;

			// Use any command queue
			vkResetCommandPool(state.logicalDevice, state.commandPool, 0);
			memory::CommandBuffer::beginCommand(commandBuffer);

			ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

			VkSubmitInfo end_info = {};
			end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			end_info.commandBufferCount = 1;
			end_info.pCommandBuffers = &commandBuffer;
			memory::CommandBuffer::endCommand(commandBuffer);
			vkQueueSubmit(state.queues.graphicsQueue, 1, &end_info, VK_NULL_HANDLE);

			deviceWaitIdle();
			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}
	}
	void createDescriptorPool(const VkDevice& logicalDevice)
	{
		const auto& pool_sizes = vulkan::initializers::descriptorPoolSize();

		uint32 size = static_cast<uint32>(pool_sizes.size());
		VkDescriptorPoolCreateInfo poolInfo = vulkan::initializers::descriptorPoolCreateInfo(
			pool_sizes.data(), 
			size, 
			1000 * size, 
			VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);

		VkResult err = vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &state.descriptorPool);
		check_vk_result(err, "failed to create descriptor pool!");
	}
	void createDescriptorSetLayout(const VkDevice& logicalDevice)
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding = vulkan::initializers::descriptorSetLayoutBinding(
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 
			VK_SHADER_STAGE_VERTEX_BIT, 
			0,
			1);

		VkDescriptorSetLayoutBinding samplerLayoutBinding = vulkan::initializers::descriptorSetLayoutBinding(
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 
			VK_SHADER_STAGE_FRAGMENT_BIT, 
			1, 
			1);

		std::vector<VkDescriptorSetLayoutBinding> bindings = { uboLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo = vulkan::initializers::descriptorSetLayoutCreateInfo(bindings);

		VkResult err = vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &state.descriptorSetLayout);
		check_vk_result(err, "failed to create descriptor set layout!");
	}
	void recreateSwapChain(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkSurfaceKHR& surface, GLFWwindow* window)
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(window, &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(window, &width, &height);
			glfwWaitEvents();
		}
		deviceWaitIdle();

		cleanupSwapChain(logicalDevice, state.swapChain);
		createSwapChain(physicalDevice, logicalDevice, surface, state.window->getWidth(), state.window->getHeight());
		createFramebuffers(logicalDevice, state.renderPass, state.swapChainExtent, state.swapChainFrames);
	}	
	void cleanupSwapChain(const VkDevice& logicalDevice, const VkSwapchainKHR& swapchain)
	{
		for (const auto& frame : state.swapChainFrames)
		{
			memory::destroyImageView(logicalDevice, frame.imageView);
			destroyFramebuffer(logicalDevice, frame.framebuffer);
			destroyFence(logicalDevice, frame.inFlightFence);
			destroySemaphore(logicalDevice, frame.imageAvailableSemaphore);
			destroySemaphore(logicalDevice, frame.renderFinishedSemaphore);
		}
		destroySwapChain(logicalDevice, swapchain);
	}
	void presentFrame(const uint32& imageIndex, VkSemaphore* signalSemaphores)
	{
		VkSwapchainKHR swapChains[] = { state.swapChain };

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		VkResult err = vkQueuePresentKHR(state.queues.presentQueue, &presentInfo);

		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR || state.framebufferResized)
		{
			state.framebufferResized = false;
			recreateSwapChain(state.physicalDevice, state.logicalDevice, state.surface, state.windowPtr);
		}
		else check_vk_result(err, "failed to present swap chain image!");
	}
	void beginFrame() 
	{
		// Stage 1. ACQUIRE IMAGE FROM SWAPCHAIN
		lockFences(state.logicalDevice, 1, state.swapChainFrames[CURRENT_FRAME].inFlightFence);

		uint32 imageIndex;
		VkResult result = vkAcquireNextImageKHR(state.logicalDevice, state.swapChain, UINT64_MAX, state.swapChainFrames[CURRENT_FRAME].imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain(state.physicalDevice, state.logicalDevice, state.surface, state.windowPtr);
			return;
		}
		else
		{
			VOXEL_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "failed to acquire swap chain image!")
		}

		VoxelEngine::renderer::UniformBufferObject ubo = {};
		const float aspectRatio = (float)state.swapChainExtent.width / state.swapChainExtent.height;

		ubo.model = glm::mat4(1.0f);
		ubo.view = FPVcamera->viewMatrix();
		ubo.proj = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 200.0f);
		ubo.proj[1][1] *= -1;

		resetFences(state.logicalDevice, 1, state.swapChainFrames[CURRENT_FRAME].inFlightFence);
		memory::CommandBuffer::reset(state.swapChainFrames[CURRENT_FRAME].commandBuffer);

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();	
	}
	void endFrame()
	{
		// Stage 2. GRAPHICS
		ImGui::Render();

		recordCommandBuffer(state.swapChainFrames[CURRENT_FRAME].commandBuffer, CURRENT_FRAME, nullptr);

		VkSemaphore signalSemaphores[] = { state.swapChainFrames[CURRENT_FRAME].renderFinishedSemaphore};
		submitToQueue(state.queues.graphicsQueue, state.swapChainFrames[CURRENT_FRAME].commandBuffer, signalSemaphores);

		// Stage 3. PRESENT
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		presentFrame(CURRENT_FRAME, signalSemaphores);

		CURRENT_FRAME = (CURRENT_FRAME + 1) % MAX_FRAMES_IN_FLIGHT;
	}
	void recordCommandBuffer(const VkCommandBuffer& commandBuffer, const uint32& imageIndex, const VoxelEngine::Scene* scene)
	{
		memory::CommandBuffer::beginCommand(commandBuffer);

		std::vector<VkClearValue> clearValues(1);
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		VkRenderPassBeginInfo renderPassBeginInfo = vulkan::initializers::renderPassBeginInfo(
			state.renderPass,
			state.swapChainFrames[imageIndex].framebuffer,
			state.swapChainExtent,
			clearValues);
		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, state.graphicsPipeline);
		
		VkViewport viewport = vulkan::initializers::viewport(state.swapChainExtent, 0.0f, 1.0f);
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor = vulkan::initializers::rect2D(state.swapChainExtent, { 0, 0 });
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		// =================== RENDER WHOLE STUFF HERE ! ===================
		ImDrawData* main_draw_data = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(main_draw_data, commandBuffer);
		// =================================================================

		vkCmdEndRenderPass(commandBuffer);
		memory::CommandBuffer::endCommand(commandBuffer);
	}
	void submitToQueue(const VkQueue& queue, const VkCommandBuffer& commandBuffer, const VkSemaphore* signalSemaphores)
	{
		VkSemaphore waitSemaphores[] = { state.swapChainFrames[CURRENT_FRAME].imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo = vulkan::initializers::submitInfo(
			waitSemaphores, 
			signalSemaphores, 
			commandBuffer, 
			waitStages);	

		VkResult err = vkQueueSubmit(queue, 1, &submitInfo, state.swapChainFrames[CURRENT_FRAME].inFlightFence);
		check_vk_result(err, "failed to submit draw command buffer! Possible reasons:\n 1) Incorrect shaders for submitting to the command queue. Please recompile your shaders!\n 2) Synchronization issues.");
	}
	void setWindow(const VoxelEngine::Window& window)
	{
		state.window = &window;
		state.windowPtr = (GLFWwindow*)(window.getNativeWindow());
		glfwSetFramebufferSizeCallback(state.windowPtr, framebufferResizeCallback);
		int success = glfwVulkanSupported();
		VOXEL_CORE_ASSERT(success, "GLFW: Vulkan Not Supported")
	}
	void setCamera(VoxelEngine::components::camera::Camera* camera)
	{
		FPVcamera = camera;
	}

	void init()
	{		
		createInstance();
		setupDebugReportMessenger(state.instance, &state.debugReportMessenger);
		state.surface = createSurface(state.instance, state.windowPtr);
		state.physicalDevice = pickPhysicalDevice(state.instance, state.surface, _enableValidationLayers);
		state.queueFamilyIndices = findQueueFamilies(state.physicalDevice, state.surface);
		state.logicalDevice = createLogicalDevice(state.physicalDevice, state.surface, _enableValidationLayers);
		state.queues = getDeviceQueues(state.physicalDevice, state.logicalDevice, state.surface);

		const auto& swapChainBundle = createSwapChain(state.physicalDevice, state.logicalDevice, state.surface, state.window->getWidth(), state.window->getHeight());
		state.swapChainImageFormat = swapChainBundle.format;
		state.swapChainExtent = swapChainBundle.extent;
		state.swapChain = swapChainBundle.swapchain;
		state.swapChainFrames = swapChainBundle.frames;

		createRenderPass(state.logicalDevice, state.swapChainImageFormat);
		createDescriptorSetLayout(state.logicalDevice);
		createGraphicsPipeline(state.logicalDevice, state.renderPass, state.descriptorSetLayout);
		createCommandPool(state.physicalDevice, state.logicalDevice);	
		
		createFramebuffers(state.logicalDevice, state.renderPass, state.swapChainExtent, state.swapChainFrames);
		
		createDescriptorPool(state.logicalDevice);
		createCommandBuffers();
		createSyncObjects(state.logicalDevice);
		initImGui();

		VOXEL_CORE_WARN("Vulkan setup ended.")	
	}
	void deviceWaitIdle()
	{
		vkDeviceWaitIdle(state.logicalDevice);
	}
	void cleanup()
	{
		cleanupSwapChain(state.logicalDevice, state.swapChain);
		
		vkDestroyPipeline(state.logicalDevice, state.graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(state.logicalDevice, state.pipelineLayout, nullptr);
		vkDestroyRenderPass(state.logicalDevice, state.renderPass, nullptr);
		vkDestroyDescriptorPool(state.logicalDevice, state.descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(state.logicalDevice, state.descriptorSetLayout, nullptr);
		
		vkDestroyCommandPool(state.logicalDevice, state.commandPool, nullptr);
		vkDestroyDevice(state.logicalDevice, nullptr);

		if (_enableValidationLayers)
		{
			destroyDebugReportMessengerEXT(state.instance, state.debugReportMessenger, nullptr);
		}

		vkDestroySurfaceKHR(state.instance, state.surface, nullptr);
		vkDestroyInstance(state.instance, nullptr);
	}

	void render(const VoxelEngine::Scene* scene)
	{
		beginFrame();

		endFrame();
	}

	const VkDevice& getLogicalDevice()
	{
		return state.logicalDevice;
	}
	const VkPhysicalDevice& getPhysicalDevice()
	{
		return state.physicalDevice;
	}
	const VkCommandBuffer& getCommandBuffer()
	{
		return state.swapChainFrames[CURRENT_FRAME].commandBuffer;
	}
	const VkCommandPool& getCommandPool()
	{
		return state.commandPool;
	}

	// ==================== MEMORY ALLOC / DEALLOC ====================
	const VkDeviceMemory allocateMemory(const VkMemoryRequirements& requirements, const VkMemoryPropertyFlags& properties)
	{
		return memory::allocateMemory(state.physicalDevice, state.logicalDevice, requirements, properties);
	}
	void createBuffer(
		const VkDeviceSize& size,
		const VkBufferUsageFlags& usage,
		const VkMemoryPropertyFlags& properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory)
	{
		memory::createBuffer(
			state.physicalDevice,
			state.logicalDevice,
			size,
			usage,
			properties,
			buffer,
			bufferMemory);
	}
	void destroyBuffer(const VkBuffer& buffer)
	{
		memory::destroyBuffer(state.logicalDevice, buffer);
	}
	void freeDeviceMemory(const VkDeviceMemory& memory)
	{
		memory::freeDeviceMemory(state.logicalDevice, memory);
	}
	void endSingleTimeCommands(const VkCommandBuffer& commandBuffer)
	{
		memory::CommandBuffer::endCommand(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(state.queues.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(state.queues.graphicsQueue);

		memory::CommandBuffer::release(commandBuffer);
	}
	void copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const VkDeviceSize& size)
	{
		VkCommandBuffer commandBuffer = memory::beginSingleTimeCommands();

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		endSingleTimeCommands(commandBuffer);
	}
	// =============================================================
}