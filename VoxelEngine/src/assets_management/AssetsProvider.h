#pragma once
#include <core/PrimitiveTypes.h>
#include <core/Base.h>
#include "Model.h"

#if defined(VOXEL_PLATFORM_WINDOWS)
#define PATH_SEPARATOR "\\"
#else 
#define PATH_SEPARATOR "/"
#endif

#define RESOURCES_FOLDER_NAME "resources"
#define RESOURCES_DIR RESOURCES_FOLDER_NAME PATH_SEPARATOR
#define ASSET_PATH(path) RESOURCES_DIR path

using Mesh = VoxelEngine::components::mesh::Mesh;

namespace assets
{
	static class AssetsProvider final
	{
	public:
		static const VoxelEngine::SharedRef<Mesh*> loadObjMesh(const string& path);
	};
}