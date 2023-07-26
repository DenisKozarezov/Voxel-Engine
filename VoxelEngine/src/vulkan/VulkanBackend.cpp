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
	using namespace VoxelEngine::components::mesh;

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
		VkSwapchainKHR swapChain;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		std::vector<vkUtils::SwapChainFrame> swapChainFrames;
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
		for (vkUtils::SwapChainFrame& frame : state.swapChainFrames)
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
		const auto& swapChainBundle = vkInit::createSwapChain(state.physicalDevice, state.logicalDevice, state.surface, state.window->getWidth(), state.window->getHeight());
		state.swapChainImageFormat = swapChainBundle.format;
		state.swapChainExtent = swapChainBundle.extent;
		state.swapChain = swapChainBundle.swapchain;
		state.swapChainFrames = swapChainBundle.frames;
		MAX_FRAMES_IN_FLIGHT = static_cast<int>(swapChainBundle.frames.size());
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
		state.renderPass = vkInit::createRenderPass(state.logicalDevice, state.swapChainImageFormat);

		vkInit::GraphicsPipilineInputBundle graphicsInputBundle =
		{
			.logicalDevice = state.logicalDevice,
			.renderPass = state.renderPass,
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
		vkInit::createFramebuffers(state.logicalDevice, state.renderPass, state.swapChainExtent, state.swapChainFrames);

		makeCommandPool(state.physicalDevice, state.logicalDevice);

		makeCommandBuffers();

		state.descriptorPool = vkInit::createDescriptorPool(state.logicalDevice);

		makeFrameResources(state.logicalDevice);
	}
	void makeFrameResources(const VkDevice& logicalDevice)
	{
		int i = 0;
		for (vkUtils::SwapChainFrame& frame : state.swapChainFrames)
		{
			frame.imageAvailableSemaphore = vkInit::createSemaphore(logicalDevice);
			frame.renderFinishedSemaphore = vkInit::createSemaphore(logicalDevice);
			frame.inFlightFence = vkInit::createFence(logicalDevice);
			frame.descriptorSet = vkInit::allocateDescriptorSet(logicalDevice, state.descriptorPool, state.descriptorSetLayout);

			frame.makeDescriptorResources(state.physicalDevice, logicalDevice);
			
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

		cleanupSwapChain(logicalDevice, state.swapChain);
		makeSwapChain();
		vkInit::createFramebuffers(logicalDevice, state.renderPass, state.swapChainExtent, state.swapChainFrames);
		makeFrameResources(logicalDevice);
		makeCommandBuffers();
	}	
	void cleanupSwapChain(const VkDevice& logicalDevice, const VkSwapchainKHR& swapchain)
	{
		for (const auto& frame : state.swapChainFrames)
		{
			vkUtils::memory::destroyImageView(logicalDevice, frame.imageView);
			vkInit::destroyFramebuffer(logicalDevice, frame.framebuffer);
			vkInit::destroyFence(logicalDevice, frame.inFlightFence);
			vkInit::destroySemaphore(logicalDevice, frame.imageAvailableSemaphore);
			vkInit::destroySemaphore(logicalDevice, frame.renderFinishedSemaphore);

			frame.uniformBuffer.release();
			frame.modelBuffer.release(logicalDevice);
		}
		vkInit::destroySwapChain(logicalDevice, swapchain);
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
		else vkUtils::check_vk_result(err, "failed to present swap chain image!");
	}
	void prepareFrame(const uint32& imageIndex)
	{
		vkUtils::SwapChainFrame& frame = state.swapChainFrames[imageIndex];
		const float aspectRatio = (float)state.swapChainExtent.width / state.swapChainExtent.height;

		vkUtils::UniformBufferObject ubo =
		{
			.view = FPVcamera->viewMatrix(),
			.proj = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 200.0f),
		};
		ubo.proj[1][1] *= -1;
		ubo.viewproj = ubo.proj * ubo.view;

		frame.uniformBuffer.setData(&ubo, sizeof(ubo));

		size_t i = 0;
		for (const auto& vertex : state.vertexManager->vertices)
		{
			frame.modelTransforms[i++] = glm::translate(glm::mat4(1.0f), vertex.pos);
		}
		memcpy(frame.modelBufferMappedMemory, frame.modelTransforms.data(), i * sizeof(glm::mat4(1.0f)));

		frame.writeDescriptorSet(state.logicalDevice);
	}
	void prepareScene(const VkCommandBuffer& commandBuffer, const VoxelEngine::Scene* scene)
	{
		VkBuffer vertexBuffers[] = { *state.vertexManager->vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	}
	void beginFrame() 
	{
		// Stage 1. ACQUIRE IMAGE FROM SWAPCHAIN
		vkInit::lockFences(state.logicalDevice, 1, state.swapChainFrames[CURRENT_FRAME].inFlightFence);

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

		vkInit::resetFences(state.logicalDevice, 1, state.swapChainFrames[CURRENT_FRAME].inFlightFence);
		vkUtils::memory::CommandBuffer::reset(state.swapChainFrames[CURRENT_FRAME].commandBuffer);

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();	
	}
	void endFrame()
	{
		// Stage 2. GRAPHICS
		ImGui::Render();

		prepareFrame(CURRENT_FRAME);

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
		vkUtils::memory::CommandBuffer::beginCommand(commandBuffer);

		std::vector<VkClearValue> clearValues(1);
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		VkRenderPassBeginInfo renderPassBeginInfo = vkInit::renderPassBeginInfo(
			state.renderPass,
			state.swapChainFrames[imageIndex].framebuffer,
			state.swapChainExtent,
			clearValues);
		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, state.pipelineLayout, 0, 1, &state.swapChainFrames[imageIndex].descriptorSet, 0, nullptr);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, state.graphicsPipeline);
		
		VkViewport viewport = vkInit::viewport(state.swapChainExtent, 0.0f, 1.0f);
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor = vkInit::rect2D(state.swapChainExtent, { 0, 0 });
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		// =================== RENDER WHOLE STUFF HERE ! ===================
		prepareScene(commandBuffer, scene);

		uint32 vertexCount = state.vertexManager->sizes[MeshType::Triangle];
		uint32 firstVertex = state.vertexManager->offsets[MeshType::Triangle];
		renderSceneObjects(commandBuffer, vertexCount, 1, firstVertex, 0);
		
		vertexCount = state.vertexManager->sizes[MeshType::Square];
		firstVertex = state.vertexManager->offsets[MeshType::Square];
		renderSceneObjects(commandBuffer, vertexCount, 1, firstVertex, 0);

		ImDrawData* main_draw_data = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(main_draw_data, commandBuffer);
		// =================================================================

		vkCmdEndRenderPass(commandBuffer);
		vkUtils::memory::CommandBuffer::endCommand(commandBuffer);
	}
	void submitToQueue(const VkQueue& queue, const VkCommandBuffer& commandBuffer, const VkSemaphore* signalSemaphores)
	{
		VkSemaphore waitSemaphores[] = { state.swapChainFrames[CURRENT_FRAME].imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo = vkInit::submitInfo(
			waitSemaphores, 
			signalSemaphores, 
			commandBuffer, 
			waitStages);	

		VkResult err = vkQueueSubmit(queue, 1, &submitInfo, state.swapChainFrames[CURRENT_FRAME].inFlightFence);
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
		cleanupSwapChain(state.logicalDevice, state.swapChain);

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

		std::vector<Vertex> vertices = {
			{{0.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}
		};
		state.vertexManager->concatMesh(MeshType::Triangle, vertices);

		state.vertexManager->finalize(state.physicalDevice, state.logicalDevice);
	}

	void renderSceneObjects(
		const VkCommandBuffer& commandBuffer, 
		const uint32& vertexCount, 
		const uint32& instanceCount, 
		const uint32& firstVertex, 
		const uint32& firstInstance)
	{
		vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
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