#pragma once
#include <core/PrimitiveTypes.h>

namespace VoxelEngine::renderer
{
	enum RenderMode
	{
		Solid,
		Wireframe
	};

	struct RenderSettings
	{
		RenderMode renderMode = RenderMode::Solid;
		bool showNormals = false;
	};

}