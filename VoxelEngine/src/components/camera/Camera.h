#pragma once
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <core/PrimitiveTypes.h>
#include <core/Base.h>

namespace VoxelEngine::components::camera
{
	enum class CameraMovement : byte { Forward, Backward, Left, Right };
	enum ProjectionType : byte { Perspective, Orthographic, Isometric };

	class Camera
	{
	private:
		constexpr static float defaultSpeed = 5.0f;
		constexpr static float defaultSensitivity = 0.1f;
		constexpr static float defaultZoom = 45.0f;
	protected:
		glm::vec3 m_position;
		float m_movementSpeed;
		float m_mouseSensitivity;
		float m_zoom;
		float m_nearClip = 0.1f, m_farClip = 256.0f;
		float m_aspectRatio;
	public:
		Camera() noexcept = delete;
		Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f)) : 
			m_position(position), 
			m_aspectRatio(3.0f / 2),
			m_movementSpeed(defaultSpeed),
			m_mouseSensitivity(defaultSensitivity),
			m_zoom(defaultZoom) { }
		Camera(Camera const&) noexcept = delete;
		Camera(Camera&&) noexcept = delete;
		Camera& operator=(Camera const& rhs) noexcept = delete;
		Camera& operator=(Camera&& rhs) noexcept = delete;

		INLINE const glm::vec3 getPosition() const { return m_position; }
		virtual INLINE const glm::mat4 viewMatrix() const = 0;
		virtual INLINE const glm::mat4 projectionMatrix() const = 0;

		INLINE void setAspectRatio(const float& aspectRatio) { m_aspectRatio = aspectRatio; }
		INLINE void setPosition(const glm::vec3& position) { m_position = position; }
		INLINE void setClips(const float& nearClip, const float& farClip) { m_nearClip = nearClip, m_farClip = farClip; }
		INLINE void setSpeed(const float& speed) { m_movementSpeed = speed; }

		virtual void processKeyboard(const CameraMovement& direction, const float& deltaTime) = 0;
		virtual void processMouse(const float& xOffset, const float& yOffset, const bool& constrainPitch = true) = 0;
	
		virtual ~Camera() noexcept = default;
	};
}