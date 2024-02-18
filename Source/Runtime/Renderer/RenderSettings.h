#pragma once

namespace VoxelEngine::renderer
{
	enum RenderMode
	{
		Solid,
		Wireframe,
		Normals
	};

	struct RenderSettings
	{
		RenderMode renderMode = RenderMode::Solid;
		bool showEditorGrid = true;
		bool showOctree = false;
		bool showNormalsLines = false;
	};
}