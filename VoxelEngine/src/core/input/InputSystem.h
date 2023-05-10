#pragma once
#include "events/KeyCode.h"
#include "events/MouseCode.h"
#include <glm/vec2.hpp>

namespace VoxelEngine::input
{
	static class InputSystem
	{
	public:
		static const bool& isKeyPressed(KeyCode key);
		static const bool& isMouseButtonPressed(MouseCode button);
		static const glm::vec2 getMousePosition();
		static const float& getMouseX();
		static const float& getMouseY();
	};
}