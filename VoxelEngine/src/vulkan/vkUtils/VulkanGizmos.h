#pragma once
#include <glm/vec3.hpp>
#include <vulkan/vkUtils/VulkanSwapChainFrame.h>

namespace utils
{
	class Gizmos final
	{
	public:
		static void init(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice);
		static void startBatch();
		static void drawLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f));
		static void drawWireframeCube(const glm::vec3& center, const glm::vec3& size, const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f));
		static void drawWireframeCircle(const glm::vec3& center, const float& radius = 1.0f, const glm::vec3& axis = glm::vec3(0.0f, 1.0f, 0.0f));
		static void onGizmosDraw(const vkUtils::SwapChainFrame& frame);
		static void release();
	};
}