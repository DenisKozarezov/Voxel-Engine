#include "VulkanBackend.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include "vkInit/VulkanInstance.h"
#include "vkInit/VulkanDevice.h"
#include "vkInit/VulkanFramebuffer.h"
#include "vkInit/VulkanPipeline.h"
#include "vkInit/VulkanSync.h"
#include "vkInit/VulkanDescriptors.h"
#include "vkInit/VulkanUIOverlay.h"
#include "vkInit/VulkanCommand.h"
#include "vkPipelines/VulkanPipelines.h"
#include "vkUtils/VulkanUniformBuffer.h"
#include "vkUtils/VulkanStatistics.h"
#include "core/renderer/VertexManager.h"
#include "assets_management/AssetsProvider.h"
#include "core/Application.h"
#include "threading/ThreadPool.h"

namespace vulkan
{	
	using namespace VoxelEngine::components;

	struct VulkanState
	{
		GLFWwindow* windowPtr;
		VoxelEngine::renderer::VertexManager* vertexManager;
		
		// Instance-related variables
		const VoxelEngine::Window* window;
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugUtilsMessenger;
		VkDebugReportCallbackEXT debugReportMessenger;
		VkSurfaceKHR surface;
		
		// Device-related variables
		VkDevice logicalDevice;
		VkPhysicalDevice physicalDevice;
		vkInit::DeviceQueues queues;
		vkInit::SwapChainBundle swapChainBundle;
		VkSampleCountFlagBits msaaSamples;
		VkPhysicalDeviceLimits deviceLimits;

		vkUtils::QueueFamilyIndices queueFamilyIndices;

		// Command-related variables
		VkCommandPool commandPool;

		// Pipeline-related variables
		VkPipelineLayout pipelineLayout;
		VkPipelineCache pipelineCache;
		vkInit::Pipelines pipelines;
		VkRenderPass renderPass;
		VkQueryPool queryPool;
		vkInit::UIOverlay UIOverlay;

		// Descriptors
		VkDescriptorPool descriptorPool;
		VkDescriptorSetLayout descriptorSetLayout;
		
		bool framebufferResized = false;
	} state;

	VoxelEngine::threading::ThreadPool* threadPool;
	const camera::Camera* FPVcamera;
	std::vector<glm::vec3> objectsToRender;
	VoxelEngine::renderer::RenderSettings renderSettings;
	VoxelEngine::renderer::RenderFrameStats renderFrameStats;
	vkUtils::memory::Buffer instances;
	double mouseX = 0, mouseY = 0;
	struct 
	{
		bool left = false;
		bool right = false;
		bool middle = false;
	} mouseButtons;

	static int MAX_FRAMES_IN_FLIGHT = 3;
	static uint32 CURRENT_FRAME = 0;
		
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		state.framebufferResized = true;
	}

	void makeInstance()
	{
		state.instance = vkInit::createInstance();
		vkUtils::setupDebugReportMessenger(state.instance, &state.debugReportMessenger);
		state.surface = vkInit::createSurface(state.instance, state.windowPtr);
	}
	void makeDevice()
	{
		state.physicalDevice = vkInit::pickPhysicalDevice(state.instance, state.surface, &state.deviceLimits);
		state.queueFamilyIndices = vkUtils::findQueueFamilies(state.physicalDevice, state.surface);
		state.logicalDevice = vkInit::createLogicalDevice(state.physicalDevice, state.surface);
		state.queues = vkInit::getDeviceQueues(state.physicalDevice, state.logicalDevice, state.surface);
		state.msaaSamples = vkInit::findMaxSamplesCount(state.physicalDevice);
		state.queryPool = vkUtils::setupQueryPool(state.logicalDevice);

		VOXEL_CORE_TRACE("Device max samples count: {0}.", (int)state.msaaSamples);
	}
	void makeSwapChain()
	{
		state.swapChainBundle = vkInit::createSwapChain(
			state.physicalDevice, 
			state.logicalDevice, 
			state.surface, 
			state.window->getWidth(), 
			state.window->getHeight(), 
			state.msaaSamples);

		MAX_FRAMES_IN_FLIGHT = static_cast<int>(state.swapChainBundle.frames.size());

		for (vkUtils::SwapChainFrame& frame : state.swapChainBundle.frames)
		{
			frame.physicalDevice = state.physicalDevice;
			frame.logicalDevice = state.logicalDevice;
		}
	}
	void makeFramebuffers()
	{
		VkExtent2D swapChainExtent = state.swapChainBundle.extent;
		VkImageView colorView = state.swapChainBundle.colorImageView;
		VkImageView depthImageView = state.swapChainBundle.depthImageView;
		auto& frames = state.swapChainBundle.frames;

		for (auto& frame : frames)
		{
			std::vector<VkImageView> attachments = { colorView, depthImageView, frame.imageView };
			frame.framebuffer = vkInit::createFramebuffer(state.logicalDevice, state.renderPass, swapChainExtent, attachments);
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

		/*bindings.indices.push_back(1);
		bindings.types.push_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		bindings.stages.push_back(VK_SHADER_STAGE_FRAGMENT_BIT);
		bindings.counts.push_back(1);*/

		state.descriptorSetLayout = vkInit::createDescriptorSetLayout(state.logicalDevice, bindings);
	}
	void makeGraphicsPipeline()
	{
		VkFormat swapChainImageFormat = state.swapChainBundle.format;
		VkFormat depthFormat = state.swapChainBundle.depthFormat;
		state.renderPass = vkInit::createRenderPass(state.logicalDevice, swapChainImageFormat, depthFormat, state.msaaSamples);

		constexpr std::array<VkVertexInputBindingDescription, 2> bindingDescriptions =
		{
			vkInit::vertexInputBindingDescription(VERTEX_BUFFER_BIND_ID, sizeof(VoxelEngine::renderer::Vertex), VK_VERTEX_INPUT_RATE_VERTEX),
			vkInit::vertexInputBindingDescription(INSTANCE_BUFFER_BIND_ID, sizeof(vkUtils::InstanceData), VK_VERTEX_INPUT_RATE_INSTANCE)
		};
		constexpr std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions =
		{
			vkInit::vertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VoxelEngine::renderer::Vertex, VoxelEngine::renderer::Vertex::pos)),
			vkInit::vertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VoxelEngine::renderer::Vertex, VoxelEngine::renderer::Vertex::normal)),
			vkInit::vertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VoxelEngine::renderer::Vertex, VoxelEngine::renderer::Vertex::color)),
			vkInit::vertexInputAttributeDescription(INSTANCE_BUFFER_BIND_ID, 3, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vkUtils::InstanceData, vkUtils::InstanceData::pos))
		};

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::pipelineVertexInputStateCreateInfo(
			bindingDescriptions.data(),
			static_cast<uint32>(bindingDescriptions.size()),
			attributeDescriptions.data(),
			static_cast<uint32>(attributeDescriptions.size()));

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
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = vkInit::pipelineDynamicStateCreateInfo(dynamicStates);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = vkInit::pipelineLayoutCreateInfo(&state.descriptorSetLayout);

		VkResult err = vkCreatePipelineLayout(state.logicalDevice, &pipelineLayoutInfo, nullptr, &state.pipelineLayout);
		vkUtils::check_vk_result(err, "failed to create pipeline layout!");

		VOXEL_CORE_TRACE("Vulkan pipeline layout created.")

		VkGraphicsPipelineCreateInfo pipelineInfo = vkInit::pipelineCreateInfo();
		pipelineInfo.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineInfo.layout = state.pipelineLayout;
		pipelineInfo.renderPass = state.renderPass;
		pipelineInfo.subpass = 0;

		// SOLID
		std::array<VkPipelineShaderStageCreateInfo, 3> shaderStages;
		{
			vkUtils::VulkanShader vertexShader = vkUtils::VulkanShader(state.logicalDevice, ASSET_PATH("shaders/InstancedSolidVert.spv"), VK_SHADER_STAGE_VERTEX_BIT);
			vkUtils::VulkanShader fragShader = vkUtils::VulkanShader(state.logicalDevice, ASSET_PATH("shaders/SolidFragShader.spv"), VK_SHADER_STAGE_FRAGMENT_BIT);
			shaderStages[0] = vertexShader.getStage();
			shaderStages[1] = fragShader.getStage();
			pipelineInfo.pStages = shaderStages.data();		
			pipelineInfo.stageCount = 2;

			err = vkCreateGraphicsPipelines(state.logicalDevice, state.pipelineCache, 1, &pipelineInfo, nullptr, &state.pipelines.solid);
			vkUtils::check_vk_result(err, "failed to create graphics pipeline!");

			VOXEL_CORE_TRACE("Vulkan solid graphics pipeline created.")
		}

		// NORMALS
		{
			vkUtils::VulkanShader vertexShader = vkUtils::VulkanShader(state.logicalDevice, ASSET_PATH("shaders/InstancedBaseVert.spv"), VK_SHADER_STAGE_VERTEX_BIT);
			vkUtils::VulkanShader fragShader = vkUtils::VulkanShader(state.logicalDevice, ASSET_PATH("shaders/BaseFragShader.spv"), VK_SHADER_STAGE_FRAGMENT_BIT);
			vkUtils::VulkanShader geomShader = vkUtils::VulkanShader(state.logicalDevice, ASSET_PATH("shaders/editor/NormalGeomShader.spv"), VK_SHADER_STAGE_GEOMETRY_BIT);
			shaderStages[0] = vertexShader.getStage();
			shaderStages[1] = fragShader.getStage();
			shaderStages[2] = geomShader.getStage();

			pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
			pipelineInfo.stageCount = 3;
			rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			pipelineInfo.basePipelineHandle = state.pipelines.solid;
			err = vkCreateGraphicsPipelines(state.logicalDevice, state.pipelineCache, 1, &pipelineInfo, nullptr, &state.pipelines.normals);
			vkUtils::check_vk_result(err, "failed to create graphics pipeline!");

			VOXEL_CORE_TRACE("Vulkan normals graphics pipeline created.")
		}

		// WIREFRAME
		{
			vkUtils::VulkanShader vertexShader = vkUtils::VulkanShader(state.logicalDevice, ASSET_PATH("shaders/InstancedSolidVert.spv"), VK_SHADER_STAGE_VERTEX_BIT);
			vkUtils::VulkanShader fragShader = vkUtils::VulkanShader(state.logicalDevice, ASSET_PATH("shaders/editor/WireframeFragShader.spv"), VK_SHADER_STAGE_FRAGMENT_BIT);
			shaderStages[0] = vertexShader.getStage();
			shaderStages[1] = fragShader.getStage();

			pipelineInfo.stageCount = 2;
			pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
			pipelineInfo.basePipelineHandle = state.pipelines.solid;
			rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			err = vkCreateGraphicsPipelines(state.logicalDevice, state.pipelineCache, 1, &pipelineInfo, nullptr, &state.pipelines.wireframe);
			vkUtils::check_vk_result(err, "failed to create graphics pipeline!");

			VOXEL_CORE_TRACE("Vulkan wireframe graphics pipeline created.")
		}

		// EDITOR GRID
		vkUtils::VulkanShader vertexShader = vkUtils::VulkanShader(state.logicalDevice, ASSET_PATH("shaders/editor/EditorGridVert.spv"), VK_SHADER_STAGE_VERTEX_BIT);
		vkUtils::VulkanShader fragShader = vkUtils::VulkanShader(state.logicalDevice, ASSET_PATH("shaders/editor/EditorGridFrag.spv"), VK_SHADER_STAGE_FRAGMENT_BIT);
		shaderStages[0] = vertexShader.getStage();
		shaderStages[1] = fragShader.getStage();

		pipelineInfo.stageCount = 2;
		pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
		pipelineInfo.basePipelineHandle = state.pipelines.wireframe;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		err = vkCreateGraphicsPipelines(state.logicalDevice, state.pipelineCache, 1, &pipelineInfo, nullptr, &state.pipelines.editorGrid);
		vkUtils::check_vk_result(err, "failed to create graphics pipeline!");

		VOXEL_CORE_TRACE("Vulkan editor grid graphics pipeline created.")
	}
	void makeFrameResources(const VkDevice& logicalDevice)
	{
		int i = 0;
		for (vkUtils::SwapChainFrame& frame : state.swapChainBundle.frames)
		{
			frame.imageAvailableSemaphore = vkInit::createSemaphore(logicalDevice);
			frame.renderFinishedSemaphore = vkInit::createSemaphore(logicalDevice);
			frame.inFlightFence = vkInit::createFence(logicalDevice);
			frame.descriptorSet = vkInit::allocateDescriptorSet(logicalDevice, state.descriptorPool, state.descriptorSetLayout);
			//frame.viewportDescriptor = vkInit::allocateDescriptorSet(logicalDevice, state.descriptorPool, state.descriptorSetLayout);
			//frame.viewportSampler = state.viewportSampler;

			frame.makeDescriptorResources(state.deviceLimits);

			VOXEL_CORE_TRACE("Vulkan frame resources created for frame {0}.", i)
			++i;
		}
	}	
	void makeCommandBuffers()
	{
		int i = 0;
		for (vkUtils::SwapChainFrame& frame : state.swapChainBundle.frames)
		{
			frame.commandBuffer = vkUtils::memory::allocateCommandBuffer(state.commandPool);
			
			VOXEL_CORE_TRACE("Vulkan command buffer allocated for frame {0}.", i)
			++i;
		}
	}
	void finalizeSetup()
	{
		makeFramebuffers();

		state.commandPool = vkInit::createCommandPool(state.logicalDevice, state.queueFamilyIndices.graphicsFamily.value());

		makeCommandBuffers();

		state.descriptorPool = vkInit::createDescriptorPool(state.logicalDevice);

		makeFrameResources(state.logicalDevice);

		prepareInstanceData(objectsToRender);
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

		cleanupSwapChain();

		makeSwapChain();
		makeFramebuffers();
		makeFrameResources(logicalDevice);
		makeCommandBuffers();

		state.UIOverlay.resize(static_cast<uint32>(width), static_cast<uint32>(height));	
	}	
	void recordCommandBuffer(const VkCommandBuffer& commandBuffer, const uint32& imageIndex)
	{
		vkUtils::SwapChainFrame& frame = state.swapChainBundle.frames[imageIndex];
		VkExtent2D swapChainExtent = state.swapChainBundle.extent;

		std::vector<VkClearValue> clearValues(2);
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };

		vkUtils::memory::beginCommand(commandBuffer);
		VkRenderPassBeginInfo renderPassInfo = vkInit::renderPassBeginInfo(
			state.renderPass,
			frame.framebuffer,
			swapChainExtent,
			clearValues);
		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = vkInit::viewport(swapChainExtent, 0.0f, 1.0f);
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor = vkInit::rect2D(swapChainExtent, { 0, 0 });
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		// =================== RENDER WHOLE STUFF HERE ! ===================
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, state.pipelineLayout, 0, 1, &frame.descriptorSet, 0, nullptr);

		prepareScene(commandBuffer);
		switch (renderSettings.renderMode)
		{
		case 0:
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, state.pipelines.solid);
			break;
		case 1:
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, state.pipelines.wireframe);
			break;
		}

		uint32 startInstance = 0;
		uint32 instancesCount = static_cast<uint32>(objectsToRender.size());
		renderSceneObjects(commandBuffer, mesh::MeshType::Cube, startInstance, instancesCount);

		if (renderSettings.showNormals)
		{
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, state.pipelines.normals);
			startInstance = 0;
			renderSceneObjects(commandBuffer, mesh::MeshType::Cube, startInstance, instancesCount);
		}

		startInstance = 0;
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, state.pipelines.editorGrid);
		renderSceneObjects(commandBuffer, mesh::MeshType::Square, startInstance, 1);

		drawUI(commandBuffer);

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
		vkUtils::check_vk_result(err, "failed to submit draw command buffer!");
	
		vkUtils::getQueryResults(state.logicalDevice, state.queryPool);
	}
	void cleanupSwapChain()
	{
		state.swapChainBundle.release(state.logicalDevice);	
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

		VkResult err = vkQueuePresentKHR(state.queues.presentQueue, &presentInfo);

		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR || state.framebufferResized)
		{
			state.framebufferResized = false;
			recreateSwapChain(state.physicalDevice, state.logicalDevice, state.surface, state.windowPtr);
		}
		else vkUtils::check_vk_result(err, "failed to present swap chain image!");
	}
	void prepareFrame(const uint32& imageIndex)
	{
		vkUtils::SwapChainFrame& frame = state.swapChainBundle.frames[imageIndex];

		vkUtils::UniformBufferObject ubo =
		{
			.view = FPVcamera->viewMatrix(),
			.proj = FPVcamera->projectionMatrix(),
			.viewproj = ubo.proj * ubo.view,
			.lightPos = FPVcamera->getPosition()
		};
		memcpy(frame.uniformBuffers.view.mappedMemory, &ubo, sizeof(ubo));

		frame.writeDescriptorSet();
	}
	void prepareScene(const VkCommandBuffer& commandBuffer)
	{
		VkBuffer vertexBuffer = *state.vertexManager->vertexBuffer;
		VkBuffer instanceBuffer = instances.buffer;
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, VERTEX_BUFFER_BIND_ID, 1, &vertexBuffer, offsets);
		vkCmdBindVertexBuffers(commandBuffer, INSTANCE_BUFFER_BIND_ID, 1, &instanceBuffer, offsets);
		vkCmdBindIndexBuffer(commandBuffer, *state.vertexManager->indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}
	void beginFrame() 
	{
		vkUtils::SwapChainFrame& frame = state.swapChainBundle.frames[CURRENT_FRAME];

		// Stage 1. ACQUIRE IMAGE FROM SWAPCHAIN
		vkInit::lockFences(state.logicalDevice, 1, frame.inFlightFence);

		uint32 imageIndex;
		VkResult result = vkAcquireNextImageKHR(state.logicalDevice, state.swapChainBundle.swapchain, UINT64_MAX, frame.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain(state.physicalDevice, state.logicalDevice, state.surface, state.windowPtr);
			return;
		}
		else
		{
			VOXEL_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "failed to acquire swap chain image!")
		}

		vkInit::resetFences(state.logicalDevice, 1, frame.inFlightFence);
		vkUtils::memory::resetCommandBuffer(frame.commandBuffer);
	}
	void endFrame()
	{
		vkUtils::SwapChainFrame& frame = state.swapChainBundle.frames[CURRENT_FRAME];

		// Stage 2. GRAPHICS
		prepareFrame(CURRENT_FRAME);

		recordCommandBuffer(frame.commandBuffer, CURRENT_FRAME);

		VkSemaphore signalSemaphores[] = { frame.renderFinishedSemaphore};
		submitToQueue(state.queues.graphicsQueue, frame.commandBuffer, signalSemaphores);

		// Stage 3. PRESENT
		presentFrame(CURRENT_FRAME, signalSemaphores);

		CURRENT_FRAME = (CURRENT_FRAME + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void drawUI(const VkCommandBuffer& commandBuffer)
	{
		if (state.UIOverlay.visible) 
		{
			VkExtent2D extent = state.swapChainBundle.extent;

			VkViewport viewport = vkInit::viewport(extent, 0.0f, 1.0f);
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

			VkRect2D scissor = vkInit::rect2D(extent, { 0, 0 });
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			state.UIOverlay.draw(commandBuffer);
		}
	}
	void updateUIOverlay()
	{
		ImGuiIO& io = ImGui::GetIO();

		io.DisplaySize = ImVec2((float)state.swapChainBundle.extent.width, (float)state.swapChainBundle.extent.height);
		io.DeltaTime = VoxelEngine::Application::getInstance().getDeltaTime();
		io.MousePos = ImVec2(mouseX, mouseY);		
		io.MouseDown[0] = mouseButtons.left && state.UIOverlay.visible;
		io.MouseDown[1] = mouseButtons.right && state.UIOverlay.visible;
		io.MouseDown[2] = mouseButtons.middle && state.UIOverlay.visible;

		if (state.UIOverlay.update() || state.UIOverlay.updated)
		{
			state.UIOverlay.updated = false;
		}
	}
	
	VkDescriptorSet getCurrentDescriptorSet()
	{
		return nullptr;
	}

	void setWindow(const Window& window)
	{
		state.window = &window;
		state.windowPtr = (GLFWwindow*)(window.getNativeWindow());
		glfwSetFramebufferSizeCallback(state.windowPtr, framebufferResizeCallback);
		int success = glfwVulkanSupported();
		VOXEL_CORE_ASSERT(success, "GLFW: Vulkan Not Supported");

		glfwSetCursorPosCallback(state.windowPtr, [](GLFWwindow* window, double xpos, double ypos)
		{
			mouseX = xpos;
			mouseY = ypos;
		});
		glfwSetMouseButtonCallback(state.windowPtr, [](GLFWwindow* window, int button, int action, int mods)
		{
			switch (action)
			{
				case GLFW_PRESS:
				{
					switch (button)
					{
					case GLFW_MOUSE_BUTTON_1:
						mouseButtons.left = true;
						break;
					case GLFW_MOUSE_BUTTON_2:
						mouseButtons.right = true;
					case GLFW_MOUSE_BUTTON_3:
						mouseButtons.middle = true;
					}
					break;
				}
				case GLFW_RELEASE:
				{
					switch (button)
					{
					case GLFW_MOUSE_BUTTON_1:
						mouseButtons.left = false;
						break;
					case GLFW_MOUSE_BUTTON_2:
						mouseButtons.right = false;
					case GLFW_MOUSE_BUTTON_3:
						mouseButtons.middle = false;
					}
				}
			}
		});
	}
	void setCamera(const components::camera::Camera& camera)
	{
		FPVcamera = &camera;
	}
	void submitRenderables(const std::vector<glm::vec3> objects)
	{
		objectsToRender = objects;
	}
	void init()
	{
		makeInstance();

		makeDevice();

		makeSwapChain();

		makeDescriptorSetLayout();
		
		makeGraphicsPipeline();
		
		finalizeSetup();

		initImGui();

		VOXEL_CORE_TRACE("Vulkan setup ended.")	

		makeAssets();
	}
	void initImGui()
	{
		state.UIOverlay.logicalDevice = state.logicalDevice;
		state.UIOverlay.physicalDevice = state.physicalDevice;
		state.UIOverlay.commandPool = state.commandPool;
		state.UIOverlay.queue = state.queues.graphicsQueue;
		state.UIOverlay.rasterizationSamples = state.msaaSamples;

		vkUtils::VulkanShader vertShader = vkUtils::VulkanShader(state.logicalDevice, ASSET_PATH("shaders/editor/UIOverlayVert.spv"), VK_SHADER_STAGE_VERTEX_BIT);
		vkUtils::VulkanShader fragShader = vkUtils::VulkanShader(state.logicalDevice, ASSET_PATH("shaders/editor/UIOverlayFrag.spv"), VK_SHADER_STAGE_FRAGMENT_BIT);
		state.UIOverlay.shaders =
		{
			vertShader.getStage(),
			fragShader.getStage()
		};
		state.UIOverlay.prepareResources();
		state.UIOverlay.preparePipeline(
			state.pipelineCache, 
			state.renderPass, 
			state.swapChainBundle.format, 
			state.swapChainBundle.depthFormat);

		//vertShader.unbind();
		//fragShader.unbind();
	}
	void deviceWaitIdle()
	{
		vkDeviceWaitIdle(state.logicalDevice);
	}
	void cleanup()
	{
		delete threadPool;

		cleanupSwapChain();
		instances.release();

		delete state.vertexManager;
		
		state.pipelines.release(state.logicalDevice);
		vkDestroyPipelineLayout(state.logicalDevice, state.pipelineLayout, nullptr);
		vkDestroyRenderPass(state.logicalDevice, state.renderPass, nullptr);
		vkDestroyDescriptorPool(state.logicalDevice, state.descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(state.logicalDevice, state.descriptorSetLayout, nullptr);
		vkDestroyQueryPool(state.logicalDevice, state.queryPool, nullptr);

		state.UIOverlay.freeResources();

		vkDestroyCommandPool(state.logicalDevice, state.commandPool, nullptr);
		vkDestroyDevice(state.logicalDevice, nullptr);

		if (vkUtils::_enableValidationLayers)
		{
			vkUtils::destroyDebugReportMessengerEXT(state.instance, state.debugReportMessenger, nullptr);
		}

		vkDestroySurfaceKHR(state.instance, state.surface, nullptr);
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
	void resetFrameStats()
	{
		renderFrameStats.drawCalls = 0;
	}
	
	void prepareInstanceData(const std::vector<glm::vec3>& vertices)
	{
		std::vector<vkUtils::InstanceData> instanceData;
		instanceData.reserve(vertices.size());
		for (const auto& vertex : vertices)
		{
			instanceData.push_back(vkUtils::InstanceData
			{
				.pos = vertex
			});
		}

		VkDeviceSize instanceBufferSize = sizeof(vkUtils::InstanceData) * instanceData.size();
		instances = vkUtils::memory::createBuffer(
			state.physicalDevice,
			state.logicalDevice,
			instanceBufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

		vkUtils::memory::mapMemory(
			state.logicalDevice, 
			instances.bufferMemory, 
			0, 
			instanceBufferSize, 
			0, 
			instanceData.data());
	}
	void makeAssets()
	{
		state.vertexManager = new VoxelEngine::renderer::VertexManager;

		const auto& mesh = mesh::VoxelMesh();
		state.vertexManager->concatMesh(mesh::MeshType::Cube, mesh.vertices, mesh.indices);

		const auto& quad = mesh::SquareMesh();
		state.vertexManager->concatMesh(mesh::MeshType::Square, quad.vertices, quad.indices);

		state.vertexManager->finalize(state.physicalDevice, state.logicalDevice);

		renderFrameStats.pipelineStatNames = vkUtils::pipelineStatNames.data();
		renderFrameStats.pipelineStats = vkUtils::pipelineStats.data();

		uint32 instancesCount = static_cast<uint32>(objectsToRender.size());

		renderFrameStats.indices = instancesCount * static_cast<uint32>(mesh.indices.size());
		renderFrameStats.vertices = instancesCount * static_cast<uint32>(mesh.vertices.size());
		renderFrameStats.triangles = instancesCount * 12;
		renderFrameStats.instances = instancesCount;
	}
	void renderSceneObjects(
		const VkCommandBuffer& commandBuffer,
		const components::mesh::MeshType& objectType,
		uint32& startInstance,
		const uint32& instanceCount)
	{
		int indexCount = state.vertexManager->indexCounts.find(objectType)->second;
		int firstIndex = state.vertexManager->firstIndices.find(objectType)->second;

		vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, 0, startInstance);
	
		startInstance += instanceCount;
		renderFrameStats.drawCalls++;
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
		return state.swapChainBundle.frames[CURRENT_FRAME].commandBuffer;
	}
	const VkCommandPool& getCommandPool()
	{
		return state.commandPool;
	}

	// ==================== MEMORY ALLOC / DEALLOC ====================
	void copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const VkDeviceSize& size)
	{
		VkCommandBuffer commandBuffer = vkUtils::memory::beginSingleTimeCommands(state.commandPool);

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		endSingleTimeCommands(commandBuffer);
	}
	void endSingleTimeCommands(const VkCommandBuffer& commandBuffer)
	{
		vkUtils::memory::endCommand(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(state.queues.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(state.queues.graphicsQueue);

		vkUtils::memory::releaseCommandBuffer(commandBuffer, state.commandPool);
	}
	
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) 
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
	void copyImage(VkDevice device, VkCommandPool cmdPool, VkImage srcImageId, VkImage dstImageId, uint32_t width, uint32_t height)
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

		vkQueueSubmit(state.queues.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(state.queues.graphicsQueue);

		vkUtils::memory::releaseCommandBuffer(cmdBuffer, cmdPool);
	}
	void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) 
	{
		VkCommandBuffer commandBuffer = vkUtils::memory::beginSingleTimeCommands(state.commandPool);

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
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

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