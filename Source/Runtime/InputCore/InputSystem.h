#pragma once
#include "Events/KeyCode.h"
#include "Events/MouseCode.h"
#include <glm/vec2.hpp>
#include <Core/CoreDefines.h>

namespace VoxelEngine::input
{
	static class InputSystem
	{
	public:
		static bool isKeyPressed(const KeyCode& key);
		static bool isMouseButtonPressed(const MouseCode& button);
		static bool isKeyReleased(const KeyCode& key);
		static bool isMouseButtonReleased(const MouseCode& button);
		static glm::vec2 getMousePosition();
		FORCE_INLINE static float getMouseX() { return getMousePosition().x; }
		FORCE_INLINE static float getMouseY() { return getMousePosition().y; }
	};
}