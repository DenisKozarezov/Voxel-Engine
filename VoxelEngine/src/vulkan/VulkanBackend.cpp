#include "VulkanBackend.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include "vkInit/VulkanInstance.h"
#include "vkInit/VulkanDevice.h"
#include "vkInit/VulkanFramebuffer.h"
#include "vkInit/VulkanPipeline.h"
#include "vkInit/VulkanSync.h"
#include "vkInit/VulkanDescriptors.h"
#include "vkUtils/VulkanCommandBuffer.h"
#include "vkUtils/VulkanUniformBuffer.h"
#include "core/renderer/VertexManager.h"
#include "assets_management/AssetsProvider.h"

namespace vulkan
{	
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

		vkUtils::QueueFamilyIndices queueFamilyIndices;

		// Command-related variables
		VkCommandPool commandPool;

		// Pipeline-related variables
		VkPipelineLayout pipelineLayout;
		VkPipelineCache pipelineCache;
		VkPipeline graphicsPipeline;
		VkRenderPass renderPass;

		// Descriptors
		VkDescriptorPool descriptorPool;
		VkDescriptorSetLayout descriptorSetLayout;
		
		bool framebufferResized = false;
	} state;

	const VoxelEngine::components::camera::Camera* FPVcamera;
	const VoxelEngine::Scene* currentScene;

	static constexpr float FOV = 45.0f;
		
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		state.framebufferResized = true;
	}
			
	void makeCommandPool(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice)
	{
		vkUtils::QueueFamilyIndices queueFamilyIndices = vkUtils::findQueueFamilies(physicalDevice, state.surface);

		VkCommandPoolCreateInfo poolInfo = vkInit::commandPoolCreateInfo(queueFamilyIndices.graphicsFamily.value(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		VkResult err = vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &state.commandPool);
		vkUtils::check_vk_result(err, "failed to create command pool!");

		VOXEL_CORE_TRACE("Vulkan command pool created.")
	}
	void makeCommandBuffers()
	{
		int i = 0;
		for (vkUtils::SwapChainFrame& frame : state.swapChainBundle.frames)
		{
			frame.commandBuffer = vkUtils::memory::CommandBuffer::allocate();

			VOXEL_CORE_TRACE("Vulkan command buffer allocated for frame {0}.", i)
			++i;
		}
	}
	void makeInstance()
	{
		state.instance = vkInit::createInstance();
		vkUtils::setupDebugReportMessenger(state.instance, &state.debugReportMessenger);
		state.surface = vkInit::createSurface(state.instance, state.windowPtr);
	}
	void makeDevice()
	{
		state.physicalDevice = vkInit::pickPhysicalDevice(state.instance, state.surface, vkUtils::_enableValidationLayers);
		state.queueFamilyIndices = vkUtils::findQueueFamilies(state.physicalDevice, state.surface);
		state.logicalDevice = vkInit::createLogicalDevice(state.physicalDevice, state.surface, vkUtils::_enableValidationLayers);
		state.queues = vkInit::getDeviceQueues(state.physicalDevice, state.logicalDevice, state.surface);
	}
	void makeSwapChain()
	{
		state.swapChainBundle = vkInit::createSwapChain(state.physicalDevice, state.logicalDevice, state.surface, state.window->getWidth(), state.window->getHeight());

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

		vkInit::createFramebuffers(state.logicalDevice, state.renderPass, swapChainExtent, colorView, depthImageView, frames);
	}
	void makeDescriptorSetLayout()
	{
		vkInit::DescriptorSetLayoutInputBundle bindings;
		bindings.count = 2;
		bindings.indices.push_back(0);
		bindings.types.push_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		bindings.stages.push_back(VK_SHADER_STAGE_VERTEX_BIT);
		bindings.counts.push_back(1);

		bindings.indices.push_back(1);
		bindings.types.push_back(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		bindings.stages.push_back(VK_SHADER_STAGE_VERTEX_BIT);
		bindings.counts.push_back(1);
		state.descriptorSetLayout = createDescriptorSetLayout(state.logicalDevice, bindings);
	}
	void makeGraphicsPipeline()
	{
		VkFormat swapChainImageFormat = state.swapChainBundle.format;
		VkFormat depthFormat = state.swapChainBundle.depthFormat;
		VkSampleCountFlagBits msaaSamples = state.swapChainBundle.msaaSamples;
		state.renderPass = vkInit::createRenderPass(state.logicalDevice, swapChainImageFormat, depthFormat, msaaSamples);

		vkInit::GraphicsPipilineInputBundle graphicsInputBundle =
		{
			.logicalDevice = state.logicalDevice,
			.renderPass = state.renderPass,
			.msaaSamples = msaaSamples,
			.descriptorSetLayout = state.descriptorSetLayout,
			.vertexFilepath = ASSET_PATH("shaders/vert.spv"),
			.fragmentFilepath = ASSET_PATH("shaders/frag.spv")
		};
		const auto& pipelineBundle = vkInit::createGraphicsPipeline(graphicsInputBundle);
		state.graphicsPipeline = pipelineBundle.pipeline;
		state.pipelineLayout = pipelineBundle.layout;
	}
	void finalizeSetup()
	{
		makeFramebuffers();

		makeCommandPool(state.physicalDevice, state.logicalDevice);

		makeCommandBuffers();

		state.descriptorPool = vkInit::createDescriptorPool(state.logicalDevice);

		makeFrameResources(state.logicalDevice);
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

			frame.makeDescriptorResources();
			
			VOXEL_CORE_TRACE("Vulkan sync objects created for frame {0}.", i)
			++i;
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
		init_info.MSAASamples = state.swapChainBundle.msaaSamples;
		init_info.Allocator = nullptr;
		ImGui_ImplGlfw_InitForVulkan(state.windowPtr, true);
		ImGui_ImplVulkan_Init(&init_info, state.renderPass);
		
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.Fonts->AddFontDefault();
		{
			VkCommandBuffer commandBuffer = state.swapChainBundle.frames[CURRENT_FRAME].commandBuffer;

			// Use any command queue
			vkResetCommandPool(state.logicalDevice, state.commandPool, 0);
			vkUtils::memory::CommandBuffer::beginCommand(commandBuffer);

			ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

			VkSubmitInfo end_info = {};
			end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			end_info.commandBufferCount = 1;
			end_info.pCommandBuffers = &commandBuffer;
			vkUtils::memory::CommandBuffer::endCommand(commandBuffer);
			vkQueueSubmit(state.queues.graphicsQueue, 1, &end_info, VK_NULL_HANDLE);

			deviceWaitIdle();
			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}
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
		VkExtent2D swapChainExtent = state.swapChainBundle.extent;
		const float aspectRatio = (float)swapChainExtent.width / swapChainExtent.height;

		vkUtils::UniformBufferObject ubo =
		{
			.view = FPVcamera->viewMatrix(),
			.proj = glm::perspective(glm::radians(FOV), aspectRatio, 0.1f, 200.0f),
		};
		ubo.proj[1][1] *= -1;
		ubo.viewproj = ubo.proj * ubo.view;

		frame.uniformBuffer.setData(&ubo, sizeof(ubo));

		size_t i = 0;
		for (const auto& position : currentScene->vertices)
		{
			frame.modelTransforms[i++] = glm::translate(glm::mat4(1.0f), position);
		}
		memcpy(frame.modelBufferMappedMemory, frame.modelTransforms.data(), i * sizeof(glm::mat4));

		frame.writeDescriptorSet();
	}
	void prepareScene(const VkCommandBuffer& commandBuffer)
	{
		VkBuffer vertexBuffers[] = { *state.vertexManager->vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
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
		vkUtils::memory::CommandBuffer::reset(frame.commandBuffer);

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();	
	}
	void endFrame()
	{
		vkUtils::SwapChainFrame& frame = state.swapChainBundle.frames[CURRENT_FRAME];

		// Stage 2. GRAPHICS
		ImGui::Render();

		prepareFrame(CURRENT_FRAME);

		recordCommandBuffer(frame.commandBuffer, CURRENT_FRAME);

		VkSemaphore signalSemaphores[] = { frame.renderFinishedSemaphore};
		submitToQueue(state.queues.graphicsQueue, frame.commandBuffer, signalSemaphores);

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
	void recordCommandBuffer(const VkCommandBuffer& commandBuffer, const uint32& imageIndex)
	{
		vkUtils::SwapChainFrame& frame = state.swapChainBundle.frames[imageIndex];
		VkExtent2D swapChainExtent = state.swapChainBundle.extent;

		vkUtils::memory::CommandBuffer::beginCommand(commandBuffer);

		std::vector<VkClearValue> clearValues(2);
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassBeginInfo = vkInit::renderPassBeginInfo(
			state.renderPass,
			frame.framebuffer,
			swapChainExtent,
			clearValues);
		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, state.pipelineLayout, 0, 1, &frame.descriptorSet, 0, nullptr);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, state.graphicsPipeline);
		
		VkViewport viewport = vkInit::viewport(swapChainExtent, 0.0f, 1.0f);
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor = vkInit::rect2D(swapChainExtent, { 0, 0 });
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		// =================== RENDER WHOLE STUFF HERE ! ===================
		prepareScene(commandBuffer);

		uint32 startInstance = 0;
		renderSceneObjects(commandBuffer, MeshType::Polygone, startInstance, static_cast<uint32>(currentScene->vertices.size()));
		
		ImDrawData* main_draw_data = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(main_draw_data, commandBuffer);
		// =================================================================

		vkCmdEndRenderPass(commandBuffer);
		vkUtils::memory::CommandBuffer::endCommand(commandBuffer);
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
		vkUtils::check_vk_result(err, "failed to submit draw command buffer! Possible reasons:\n 1) Incorrect shaders for submitting to the command queue. Please recompile your shaders!\n 2) Synchronization issues.");
	}
	void setWindow(const VoxelEngine::Window& window)
	{
		state.window = &window;
		state.windowPtr = (GLFWwindow*)(window.getNativeWindow());
		glfwSetFramebufferSizeCallback(state.windowPtr, framebufferResizeCallback);
		int success = glfwVulkanSupported();
		VOXEL_CORE_ASSERT(success, "GLFW: Vulkan Not Supported")
	}
	void setCamera(const VoxelEngine::components::camera::Camera* camera)
	{
		FPVcamera = camera;
	}

	void setScene(const VoxelEngine::Scene* scene)
	{
		currentScene = scene;
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

		VOXEL_CORE_WARN("Vulkan setup ended.")	

		makeAssets();
	}
	void deviceWaitIdle()
	{
		vkDeviceWaitIdle(state.logicalDevice);
	}
	void cleanup()
	{
		cleanupSwapChain();

		delete state.vertexManager;
		
		vkDestroyPipeline(state.logicalDevice, state.graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(state.logicalDevice, state.pipelineLayout, nullptr);
		vkDestroyRenderPass(state.logicalDevice, state.renderPass, nullptr);
		vkDestroyDescriptorPool(state.logicalDevice, state.descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(state.logicalDevice, state.descriptorSetLayout, nullptr);
		
		vkDestroyCommandPool(state.logicalDevice, state.commandPool, nullptr);
		vkDestroyDevice(state.logicalDevice, nullptr);

		if (vkUtils::_enableValidationLayers)
		{
			vkUtils::destroyDebugReportMessengerEXT(state.instance, state.debugReportMessenger, nullptr);
		}

		vkDestroySurfaceKHR(state.instance, state.surface, nullptr);
		vkDestroyInstance(state.instance, nullptr);
	}

	void makeAssets()
	{
		state.vertexManager = new VoxelEngine::renderer::VertexManager;

		const auto& mesh = assets::AssetsProvider::loadObjMesh(ASSET_PATH("models/viking_room.obj"));

		state.vertexManager->concatMesh(MeshType::Polygone, mesh->vertices, mesh->indices);

		state.vertexManager->finalize(state.physicalDevice, state.logicalDevice);
	}

	void renderSceneObjects(
		const VkCommandBuffer& commandBuffer,
		const MeshType& objectType,
		uint32& startInstance,
		const uint32& instanceCount)
	{
		int indexCount = state.vertexManager->indexCounts.find(objectType)->second;
		int firstIndex = state.vertexManager->firstIndices.find(objectType)->second;

		vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, 0, startInstance);
	
		startInstance += instanceCount;
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
	const VkDeviceMemory allocateMemory(const VkMemoryRequirements& requirements, const VkMemoryPropertyFlags& properties)
	{
		return vkUtils::memory::allocateMemory(state.physicalDevice, state.logicalDevice, requirements, properties);
	}
	const vkUtils::memory::Buffer createBuffer(
		const VkDeviceSize& size,
		const VkBufferUsageFlags& usage,
		const VkMemoryPropertyFlags& properties)
	{
		return vkUtils::memory::createBuffer(
			state.physicalDevice,
			state.logicalDevice,
			size,
			usage,
			properties);
	}
	void destroyBuffer(const VkBuffer& buffer)
	{
		vkUtils::memory::destroyBuffer(state.logicalDevice, buffer);
	}
	void freeDeviceMemory(const VkDeviceMemory& memory)
	{
		vkUtils::memory::freeDeviceMemory(state.logicalDevice, memory);
	}
	void endSingleTimeCommands(const VkCommandBuffer& commandBuffer)
	{
		vkUtils::memory::CommandBuffer::endCommand(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(state.queues.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(state.queues.graphicsQueue);

		vkUtils::memory::CommandBuffer::release(commandBuffer);
	}
	void copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const VkDeviceSize& size)
	{
		VkCommandBuffer commandBuffer = vkUtils::memory::beginSingleTimeCommands();

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		endSingleTimeCommands(commandBuffer);
	}
	// =============================================================
}