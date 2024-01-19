#pragma once

/*----------------------------------------------------------------------------
    Low level includes.
----------------------------------------------------------------------------*/

#include "CoreTypes.h"

/*----------------------------------------------------------------------------
    Commonly used headers
----------------------------------------------------------------------------*/

#include <Core/HAL/AssetsManager/AssetsProvider.h>
#include <Core/HAL/AssetsManager/Paths.h>
#include <Core/HAL/Platform/PlatformUtils.h>
#include <InputCore/InputCore.h>

#include <Core/Internal/Application.h>
#include <Core/Internal/imgui/ImGuiLayer.h>
#include <Core/Internal/scene/Scene.h>
#include <Core/Internal/Timestep.h>
#include <Core/Internal/NonCopyable.h>

#include <Engine/Components/camera/FirstPersonCamera.h>
#include <Engine/Components/camera/OrthographicCamera.h>
#include <Engine/Components/camera/EditorCameraController.h>

using namespace VoxelEngine;