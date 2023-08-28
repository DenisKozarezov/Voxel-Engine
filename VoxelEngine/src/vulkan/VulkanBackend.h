#pragma once
#include <vulkan/vkUtils/VulkanAlloc.h>
#include <core/renderer/Renderer.h>
#include <components/mesh/Mesh.h>

namespace vulkan
{
	using namespace VoxelEngine;
	namespace mesh = components::mesh;

	void makeInstance();
	void makeDevice();
	void makeSwapChain();
	void makeFramebuffers();
	void makeDescriptorSetLayout();
	void makeGraphicsPipeline();
	void makeFrameResources();
	void makeCommandBuffers();
	void finalizeSetup();

	void recreateSwapChain();
	void recordCommandBuffer(const VkCommandBuffer& commandBuffer, const uint32& imageIndex);
	void submitToQueue(const VkQueue& queue, const VkCommandBuffer& commandBuffer, const VkSemaphore* signalSemaphores = nullptr);
	void cleanupSwapChain();
	void presentFrame(const uint32& imageIndex, VkSemaphore* signalSemaphores);
	void prepareFrame();
	void prepareScene(const VkCommandBuffer& commandBuffer);
	void prepareInstanceData();
	void beginFrame(const VoxelEngine::renderer::UniformBufferObject& ubo);
	void endFrame();
	void drawUI(const VkCommandBuffer& commandBuffer);
	void updateUIOverlay();

	void resize(const uint32& width, const uint32& height);
	void setClearColor(const glm::vec4 color);
	void setWindow(const Window& window);
	void setViewport(const int32_t& x, const int32_t& y, const uint32& width, const uint32& height);
	void submitRenderables(std::vector<glm::vec3> objects);
	void init();
	void initImGui();	
	void deviceWaitIdle();
	void cleanup();

	mesh::Material* createMaterial(const VkPipeline& matPipeline, const VkPipelineLayout& matLayout, const string& matName);
	renderer::RenderSettings& getRenderSettings();
	const renderer::RenderFrameStats& getFrameStats();
	void resetFrameStats();

	void makeAssets();
	void prepareAsset(mesh::Mesh mesh);
	void prepareAsset(const mesh::MeshTopology& topology, mesh::Mesh mesh);
	void renderSceneObjects(
		const VkCommandBuffer& commandBuffer,
		const mesh::MeshTopology& objectType,
		uint32& startInstance,
		const uint32& instanceCount);

	const VkDevice& getLogicalDevice();

	/// <summary>
	/// Returns the physical GPU device currently in use for Vulkan.
	/// </summary>
	/// <returns></returns>
	const VkPhysicalDevice& getPhysicalDevice();

	/// <summary>
	/// Returns the command buffer used in this frame in flight.
	/// </summary>
	/// <returns></returns>
	const VkCommandBuffer& getCommandBuffer();

	/// <summary>
	/// Returns a pool of commands for subsequent allocation of command buffers.
	/// </summary>
	/// <returns></returns>
	const VkCommandPool& getCommandPool();

	// ==================== MEMORY ALLOC / DEALLOC ====================
	void copyBuffer(const vkUtils::memory::Buffer& srcBuffer, vkUtils::memory::Buffer& dstBuffer, const VkDeviceSize& size);
	void endSingleTimeCommands(const VkCommandBuffer& commandBuffer);

	void copyBufferToImage(vkUtils::memory::Buffer buffer, VkImage image, uint32 width, uint32 height);
	void copyImage(VkCommandPool cmdPool, VkImage srcImageId, VkImage dstImageId, uint32 width, uint32 height);
	void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
	// ===================================================================
}