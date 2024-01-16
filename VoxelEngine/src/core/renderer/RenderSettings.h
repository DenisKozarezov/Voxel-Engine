#pragma once
#include <core/PrimitiveTypes.h>
#include <vector>

namespace VoxelEngine::renderer
{
	enum RenderMode
	{
		Solid,
		Wireframe,
		Normals
	};

	struct RenderFrameStats
	{
		uint32 drawCalls;
		uint64 triangles;
		uint64 vertices;
		uint32 indices;
		uint64 instances;
	};

	struct ShaderStats
	{
		std::vector<string> performanceStrings;
		std::vector<uint64> performanceStats;
	};

	struct RenderSettings
	{
		RenderMode renderMode = RenderMode::Solid;
		bool showEditorGrid = true;
		bool showOctree = false;
		bool showNormalsLines = false;
	};
}