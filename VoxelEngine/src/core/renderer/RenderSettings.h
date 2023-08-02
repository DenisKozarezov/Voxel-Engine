#pragma once
#include <core/PrimitiveTypes.h>

namespace VoxelEngine::renderer
{
	enum RenderMode
	{
		Solid,
		Wireframe
	};

	struct RenderFrameStats
	{
		const string* pipelineStatNames;
		uint64* pipelineStats;

		uint64 triangles;
		uint64 vertices;
		uint32 indices;
		uint64 instances;
	};

	struct RenderSettings
	{
		RenderMode renderMode = RenderMode::Solid;
		bool showNormals = false;
	};

}