#pragma once
#include "events/KeyCode.h"
#include "events/MouseCode.h"
#include <glm/vec2.hpp>

namespace VoxelEngine::input
{
	static class InputSystem
	{
	public:
		static const bool isKeyPressed(const KeyCode& key);
		static const bool isMouseButtonPressed(const MouseCode& button);
		static const bool isKeyReleased(const KeyCode& key);
		static const bool isMouseButtonReleased(const MouseCode& button);
		static const glm::vec2 getMousePosition();
		inline static const float getMouseX();
		inline static const float getMouseY();
	};
}