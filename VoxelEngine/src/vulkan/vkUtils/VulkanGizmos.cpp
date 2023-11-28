#include "VulkanGizmos.h"
#include <vulkan/VulkanBackend.h>

namespace vkUtils
{
	constexpr uint32 MAX_VERTICES = 10000;

	VulkanGizmos::VulkanGizmos(const vkInit::VulkanDevice& device)
	{
		m_renderData.linesMaterial = vkUtils::getMaterial("lines");
		m_renderData.linesBuffer = vkUtils::VulkanVertexBuffer(device, sizeof(LineVertex) * MAX_VERTICES);
		m_renderData.linesPtrStart = new LineVertex[MAX_VERTICES];
	}
	void VulkanGizmos::startBatch()
	{
		m_renderData.linesVertexCount = 0;
		m_renderData.linesPtrCurrent = m_renderData.linesPtrStart;
	}
	void VulkanGizmos::drawLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& color)
	{
		m_renderData.linesPtrCurrent->pos = point1;
		m_renderData.linesPtrCurrent->color = color;
		m_renderData.linesPtrCurrent++;

		m_renderData.linesPtrCurrent->pos = point2;
		m_renderData.linesPtrCurrent->color = color;
		m_renderData.linesPtrCurrent++;

		m_renderData.linesVertexCount += 2;
	}
	void VulkanGizmos::drawWireframeCube(const glm::vec3& center, const glm::vec3& size, const glm::vec3& color)
	{
		glm::vec3 p1 = glm::vec3(center.x - size.x * 0.5f, center.y - size.y * 0.5f, center.z + size.z * 0.5f);
		glm::vec3 p2 = glm::vec3(center.x + size.x * 0.5f, center.y - size.y * 0.5f, center.z + size.z * 0.5f);
		glm::vec3 p3 = glm::vec3(center.x + size.x * 0.5f, center.y + size.y * 0.5f, center.z + size.z * 0.5f);
		glm::vec3 p4 = glm::vec3(center.x - size.x * 0.5f, center.y + size.y * 0.5f, center.z + size.z * 0.5f);
		
		glm::vec3 p5 = glm::vec3(center.x - size.x * 0.5f, center.y - size.y * 0.5f, center.z - size.z * 0.5f);
		glm::vec3 p6 = glm::vec3(center.x + size.x * 0.5f, center.y - size.y * 0.5f, center.z - size.z * 0.5f);
		glm::vec3 p7 = glm::vec3(center.x + size.x * 0.5f, center.y + size.y * 0.5f, center.z - size.z * 0.5f);
		glm::vec3 p8 = glm::vec3(center.x - size.x * 0.5f, center.y + size.y * 0.5f, center.z - size.z * 0.5f);

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
	void VulkanGizmos::drawWireframeCircle(const glm::vec3& position, const float& radius, const glm::vec3& axis)
	{

	}
	void VulkanGizmos::onGizmosDraw(const SwapChainFrame& frame)
	{
		if (m_renderData.linesVertexCount > 0)
		{
			uint32 dataSize = (uint32)((uint8*)m_renderData.linesPtrCurrent - (uint8*)m_renderData.linesPtrStart);
			
			dataSize = glm::clamp<uint32>(dataSize, 0, MAX_VERTICES * sizeof(LineVertex));
			
			m_renderData.linesBuffer.setData(m_renderData.linesPtrStart, dataSize);
			m_renderData.linesBuffer.bind(frame.commandBuffer);
			m_renderData.linesMaterial->bind(frame.commandBuffer, frame.descriptorSet);
			vkCmdDraw(frame.commandBuffer, m_renderData.linesVertexCount, 1, 0, 0);
		}
	}
	void VulkanGizmos::onGizmosDraw()
	{
		onGizmosDraw(vulkan::getCurrentFrame());
	}
	VulkanGizmos::~VulkanGizmos()
	{
		delete[] m_renderData.linesPtrStart;
	}
}