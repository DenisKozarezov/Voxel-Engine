#pragma once
#include <vulkan/vulkan.h>
#include <unordered_map>
#include <components/mesh/Mesh.h>

namespace vkUtils
{
#define MATERIALS_MAX 5

	struct VulkanPipelineCreateInfo
	{
		VkPipelineCreateFlags flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
		VkPipelineVertexInputStateCreateInfo* vertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo* inputAssembly;
		VkPipelineViewportStateCreateInfo* viewportState;
		VkPipelineRasterizationStateCreateInfo* rasterizer;
		VkPipelineMultisampleStateCreateInfo* multisampling;
		VkPipelineColorBlendAttachmentState* colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo* colorBlending;
		VkPipelineDepthStencilStateCreateInfo* depthStencil;
		VkPipelineDynamicStateCreateInfo* dynamicState;
		VkDescriptorSetLayout* descriptorSetLayout;
		VkRenderPass* renderPass;
		VkPipelineShaderStageCreateInfo* shaderStages;
		VkPipeline basePipelineHandle;
		VkPipelineLayoutCreateInfo pipelineLayoutInfo;
		uint32 stagesCount;

		void build(
			const VkDevice& logicalDevice,
			const VkPipelineLayout& pipelineLayout,
			const VkPipelineCache& pipelineCache,
			VkPipeline* pipeline);
	};

	class VulkanMaterial : public VoxelEngine::components::mesh::IMaterial
	{
	public:
		VkPipeline pipeline = VK_NULL_HANDLE;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

		VulkanMaterial() noexcept = default;
		~VulkanMaterial() noexcept = default;

		void bind(const VkCommandBuffer& commandBuffer, const VkDescriptorSet& descriptorSet) const;
	};

	using MaterialsCache = std::unordered_map<string, VulkanMaterial>;
	
	const VulkanMaterial* createMaterial(const VkPipeline& matPipeline, const VkPipelineLayout& matLayout, const string& matName);
	
	const VulkanMaterial* getMaterial(const string& matName);
	
	void makeMaterials(
		const VkDevice& logicalDevice,
		const VkPipelineCache& pipelineCache,
		VulkanPipelineCreateInfo& pipelineInfo);
	
	void releaseMaterials(const VkDevice& logicalDevice);
}