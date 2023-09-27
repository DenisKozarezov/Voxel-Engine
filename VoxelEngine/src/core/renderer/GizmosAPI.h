#pragma once
#include <glm/vec3.hpp>
#include <core/Base.h>

namespace utils
{
	class Gizmos;

	class GizmosAPI
	{
	private:
		static GizmosAPI* s_instance;

		friend class Gizmos;
	public:
		GizmosAPI();
		virtual ~GizmosAPI() = default;

		virtual void startBatch() = 0;
		virtual void drawLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f)) = 0;
		virtual void drawWireframeCube(const glm::vec3& center, const glm::vec3& size, const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f)) = 0;
		virtual void drawWireframeCircle(const glm::vec3& center, const float& radius = 1.0f, const glm::vec3& axis = glm::vec3(0.0f, 1.0f, 0.0f)) = 0;
		virtual void onGizmosDraw() = 0;

		static UniqueRef<GizmosAPI> Create();
	};

	/*
		Facade class for debug drawing.
	*/
	static class Gizmos
	{
	public:
		static void drawLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f));
		static void drawWireframeCube(const glm::vec3& center, const glm::vec3& size, const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f));
		static void drawWireframeCircle(const glm::vec3& center, const float& radius = 1.0f, const glm::vec3& axis = glm::vec3(0.0f, 1.0f, 0.0f));
	};
}