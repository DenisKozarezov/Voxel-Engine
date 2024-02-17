#pragma once
#include "Core/CoreDefines.h"
#include "VModel.h"

namespace VoxelEngine::components::mesh
{
        struct Mesh;
}

TSharedPtr<VoxelEngine::components::mesh::Mesh> CreateTestPawn(const glm::ivec3 gridSize = {200, 200, 200});

TSharedPtr<VoxelEngine::components::mesh::Mesh> CreateTestHomotopy(const glm::ivec3 gridSize = {200, 200, 200});

vmodel::Model CreateTestSphereModel(const double& radius, const glm::ivec3 gridSize = {200, 200, 200});

vmodel::Model CreateTestPawnModel(const double& scaleFactor, const glm::ivec3& gridSize = {200, 200, 200});



