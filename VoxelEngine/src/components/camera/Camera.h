#pragma once
#include <glm/glm.hpp>

namespace VoxelEngine::components::camera
{
	enum class CameraMovement { Forward, Backward, Left, Right };

	class Camera
	{
	protected:
		glm::vec3 _position;
	public:
		Camera() noexcept = delete;
		Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f)) : _position(position) { }
		Camera(Camera const&) noexcept = delete;
		Camera(Camera&&) noexcept = delete;
		Camera& operator=(Camera const& rhs) noexcept = delete;
		Camera& operator=(Camera&& rhs) noexcept = delete;

		inline const glm::vec3 getPosition() const { return _position; }
		virtual inline const glm::mat4 viewMatrix() const = 0;
		virtual inline const glm::mat4 projectionMatrix(const float& aspectRatio) const = 0;

		virtual void processKeyboard(const CameraMovement& direction, const float& deltaTime) = 0;
		virtual void processMouse(const float& xOffset, const float& yOffset, const bool& constrainPitch = true) = 0;
	
		~Camera() noexcept = default;
	};
}