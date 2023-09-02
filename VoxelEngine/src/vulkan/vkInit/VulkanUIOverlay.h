#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <iomanip>

#include "../vkUtils/VulkanAlloc.h"
#include "../vkUtils/VulkanImage.h"
#include <assets_management/AssetsProvider.h>
#include "vulkan/vkUtils/VulkanValidation.h"
#include "vulkan/vkInit/VulkanInitializers.h"
#include <glm/vec2.hpp>
#include <imgui.h>

namespace vkInit
{
	class UIOverlay
	{
	public:
		VkQueue queue;

		VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		uint32_t subpass = 0;

		VkDevice logicalDevice;
		VkPhysicalDevice physicalDevice;
		VkCommandPool commandPool;
		vkUtils::memory::Buffer vertexBuffer;
		vkUtils::memory::Buffer indexBuffer;
		int32_t vertexCount = 0;
		int32_t indexCount = 0;

		std::vector<VkPipelineShaderStageCreateInfo> shaders;

		VkDescriptorPool descriptorPool;
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorSet descriptorSet;
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;

		VkDeviceMemory fontMemory = VK_NULL_HANDLE;
		VkImage fontImage = VK_NULL_HANDLE;
		VkImageView fontView = VK_NULL_HANDLE;
		VkSampler sampler;

		struct PushConstBlock {
			glm::vec2 scale;
			glm::vec2 translate;
		} pushConstBlock;

		bool visible = true;
		bool updated = false;
		float scale = 1.0f;

		UIOverlay() noexcept = default;
		~UIOverlay() noexcept = default;

		void preparePipeline(const VkPipelineCache pipelineCache, const VkRenderPass renderPass, const VkFormat colorFormat, const VkFormat depthFormat);
		void prepareResources();

		bool update();
		void draw(const VkCommandBuffer commandBuffer);
		void resize(uint32_t width, uint32_t height);

		void freeResources();
	};
}
