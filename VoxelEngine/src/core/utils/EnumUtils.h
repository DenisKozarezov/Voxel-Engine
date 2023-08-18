#pragma once
#include <vulkan/vulkan.h>
#include <core/renderer/RendererAPI.h>
#include <components/camera/Camera.h>
#include <spdlog/spdlog.h>

#pragma region VULKAN
constexpr const char* physicalDeviceTypeString(VkPhysicalDeviceType type)
	{
		switch (type)
		{
#define STR(r) case VK_PHYSICAL_DEVICE_TYPE_ ##r: return #r
			STR(OTHER);
			STR(INTEGRATED_GPU);
			STR(DISCRETE_GPU);
			STR(VIRTUAL_GPU);
			STR(CPU);
#undef STR
		default: return "UNKNOWN_DEVICE_TYPE";
		}
	}

constexpr const char* errorString(VkResult errorCode)
{
	switch (errorCode)
	{
#define STR(r) case VK_ ##r: return #r
		STR(NOT_READY);
		STR(TIMEOUT);
		STR(EVENT_SET);
		STR(EVENT_RESET);
		STR(INCOMPLETE);
		STR(ERROR_OUT_OF_HOST_MEMORY);
		STR(ERROR_OUT_OF_DEVICE_MEMORY);
		STR(ERROR_INITIALIZATION_FAILED);
		STR(ERROR_DEVICE_LOST);
		STR(ERROR_MEMORY_MAP_FAILED);
		STR(ERROR_LAYER_NOT_PRESENT);
		STR(ERROR_EXTENSION_NOT_PRESENT);
		STR(ERROR_FEATURE_NOT_PRESENT);
		STR(ERROR_INCOMPATIBLE_DRIVER);
		STR(ERROR_TOO_MANY_OBJECTS);
		STR(ERROR_FORMAT_NOT_SUPPORTED);
		STR(ERROR_SURFACE_LOST_KHR);
		STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
		STR(SUBOPTIMAL_KHR);
		STR(ERROR_OUT_OF_DATE_KHR);
		STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
		STR(ERROR_VALIDATION_FAILED_EXT);
		STR(ERROR_INVALID_SHADER_NV);
#undef STR
	default:
		return "UNKNOWN_ERROR";
	}
}
#pragma endregion

#pragma region GRAPHICS
constexpr const char* graphicsSpecString(const VoxelEngine::renderer::GraphicsSpec& spec)
{
	switch (spec)
	{
#define STR(x) case VoxelEngine::renderer::GraphicsSpec::##x: return #x;
		STR(None);
		STR(Vulkan);
		STR(OpenGL);
		STR(DirectX12);
	default: return "UNKNOWN_RENDERER_SPEC";
	}
}

constexpr const char* projectionTypeString(const VoxelEngine::components::camera::ProjectionType& mode)
{
	switch (mode)
	{
#define STR(x) case VoxelEngine::components::camera::ProjectionType::##x: return #x;
		STR(Perspective);
		STR(Orthographic);
	}
}
#pragma endregion

constexpr const char* logLevelString(const spdlog::level::level_enum& level)
{
	switch (level)
	{
#define STR(x) case spdlog::level::##x: return #x;
		STR(info);
		STR(warn);
		STR(critical);
		STR(err);
#undef STR
	default: return "info";
	}
}