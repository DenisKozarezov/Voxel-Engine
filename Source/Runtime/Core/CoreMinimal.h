#pragma once

// ----------- Internal modules -----------
#include <Core/Internal/Application.h>
#include <InputCore/events/KeyboardEvent.h>
#include <Core/Internal/imgui/ImGuiLayer.h>
#include <Core/Internal/imgui/ImGuiWindow.h>
#include <Core/Internal/scene/Scene.h>
#include <Core/Internal/utils/EnumUtils.h>
#include <Core/Internal/utils/PlatformUtils.h>
#include <Core/HAL/AssetsManager/AssetsProvider.h>

#include <Engine/Components/camera/FirstPersonCamera.h>
#include <Engine/Components/camera/OrthographicCamera.h>
#include <Engine/Components/camera/EditorCameraController.h>

using namespace VoxelEngine;