#pragma once
#include <core/renderer/GizmosAPI.h>
#include <vulkan/vkUtils/VulkanVertexBuffer.h>
#include <vulkan/vkUtils/VulkanMaterials.h>
#include <vulkan/vkUtils/VulkanSwapChainFrame.h>

namespace vkUtils
{
	struct LineVertex
	{
		glm::vec3 pos;
		glm::vec3 color;
	};

	struct CircleVertex
	{
		glm::vec3 pos;
		glm::vec3 axis;
		glm::vec3 color;
		float radius;
	};

	struct RenderData
	{
		uint32 linesVertexCount = 0;
		LineVertex* linesPtrStart = nullptr;
		LineVertex* linesPtrCurrent = nullptr;
		vkUtils::VulkanVertexBuffer linesBuffer;
		const vkUtils::VulkanMaterial* linesMaterial;

		uint32 circleVertexCount = 0;
		CircleVertex* circlePtrStart = nullptr;
		CircleVertex* circlePtrCurrent = nullptr;
		vkUtils::VulkanVertexBuffer circleBuffer;
		const vkUtils::VulkanMaterial* circleMaterial;
	};

	class VulkanGizmos final : public utils::GizmosAPI
	{
	private:
		RenderData m_renderData;
	public:
		VulkanGizmos(const vkInit::VulkanDevice& device);
		
		void startBatch() override;
		void drawLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f)) override;
		void drawWireframeCube(const glm::vec3& center, const glm::vec3& size, const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f)) override;
		void drawWireframeCircle(const glm::vec3& center, const float& radius = 1.0f, const glm::vec3& axis = glm::vec3(0.0f, 1.0f, 0.0f)) override;
		void onGizmosDraw(const SwapChainFrame& frame);
		void onGizmosDraw() override;

		~VulkanGizmos();
	};
}