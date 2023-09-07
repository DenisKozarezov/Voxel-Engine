#include "VulkanGizmos.h"
#include <vulkan/vkUtils/VulkanVertexBuffer.h>
#include <vulkan/vkUtils/VulkanMaterials.h>

namespace utils
{
	constexpr uint32 MAX_VERTICES = 10000;

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
	} s_renderData;

	void Gizmos::init(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice)
	{
		s_renderData.linesMaterial = vkUtils::getMaterial("lines");
		s_renderData.linesBuffer = vkUtils::VulkanVertexBuffer(physicalDevice, logicalDevice, MAX_VERTICES);
		s_renderData.linesPtrStart = new LineVertex[MAX_VERTICES];
	}
	void Gizmos::startBatch()
	{
		s_renderData.linesVertexCount = 0;
		s_renderData.linesPtrCurrent = s_renderData.linesPtrStart;
	}
	void Gizmos::drawLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& color)
	{
		s_renderData.linesPtrCurrent->pos = point1;
		s_renderData.linesPtrCurrent->color = color;
		s_renderData.linesPtrCurrent++;

		s_renderData.linesPtrCurrent->pos = point2;
		s_renderData.linesPtrCurrent->color = color;
		s_renderData.linesPtrCurrent++;

		s_renderData.linesVertexCount += 2;
	}
	void Gizmos::drawWireframeCube(const glm::vec3& position, const glm::vec3& size, const glm::vec3& color)
	{
		glm::vec3 p1 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z + size.z * 0.5f);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z + size.z * 0.5f);
		glm::vec3 p3 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z + size.z * 0.5f);
		glm::vec3 p4 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z + size.z * 0.5f);
		
		glm::vec3 p5 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z - size.z * 0.5f);
		glm::vec3 p6 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z - size.z * 0.5f);
		glm::vec3 p7 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z - size.z * 0.5f);
		glm::vec3 p8 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z - size.z * 0.5f);

		drawLine(p1, p2, color);
		drawLine(p2, p3, color);
		drawLine(p3, p4, color);
		drawLine(p4, p1, color);

		drawLine(p5, p6, color);
		drawLine(p6, p7, color);
		drawLine(p7, p8, color);
		drawLine(p8, p5, color);

		drawLine(p1, p5, color);
		drawLine(p2, p6, color);
		drawLine(p3, p7, color);
		drawLine(p4, p8, color);
	}
	void Gizmos::drawWireframeCircle(const glm::vec3& position, const float& radius, const glm::vec3& axis)
	{

	}
	void Gizmos::onGizmosDraw(const vkUtils::SwapChainFrame& frame)
	{
		if (s_renderData.linesVertexCount > 0)
		{
			uint32 dataSize = (uint32)((uint8*)s_renderData.linesPtrCurrent - (uint8*)s_renderData.linesPtrStart);
			s_renderData.linesBuffer.setData(s_renderData.linesPtrStart, dataSize);
			s_renderData.linesBuffer.bind(frame.commandBuffer, VERTEX_BUFFER_BIND_ID);
			s_renderData.linesMaterial->bind(frame.commandBuffer, frame.descriptorSet);
			vkCmdDraw(frame.commandBuffer, s_renderData.linesVertexCount, 1, 0, 0);
		}
	}
	void Gizmos::release()
	{
		s_renderData.linesBuffer.release();
		delete[] s_renderData.linesPtrStart;
	}
}