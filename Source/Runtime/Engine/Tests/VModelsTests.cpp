#include "VModelsTests.h"
#include "Engine/Components/mesh/Mesh.h"

TSharedPtr<VoxelEngine::components::mesh::Mesh> CreateTestPawn(const glm::ivec3 gridSize)
{
    vmodel::Model generatedPawn = CreateTestPawnModel(4.0, gridSize);
    auto grid = generatedPawn.get_grid();
    auto points = marching::Mesh::build_mesh_data(grid, gridSize, 0.0f, true);
    
    std::vector<Vertex> vertices;
    std::vector<uint32> indices;
    vertices.reserve(points.verts.size());
    for (size_t i = 0; i < points.verts.size(); ++i)
    {
        vertices.emplace_back(Vertex(points.verts[i], points.normals[i]));
        indices.emplace_back(static_cast<uint32>(points.indices[i]));
    }
    return MakeShared<VoxelEngine::components::mesh::Mesh>(vertices, indices);
}

TSharedPtr<VoxelEngine::components::mesh::Mesh> CreateTestHomotopy(const glm::ivec3 gridSize)
{
    auto model1 = CreateTestSphereModel(40.0, gridSize);
    auto model2 = CreateTestPawnModel(4.0, gridSize);
    auto homotopy = vmodel::Operations::homotopy(model1, model2, 0.5);
    auto points = marching::Mesh::build_mesh_data(homotopy.get_grid(), gridSize, 0.0f, true);
    
    std::vector<Vertex> vertices;
    std::vector<uint32> indices;
    vertices.reserve(points.verts.size());
    for (size_t i = 0; i < points.verts.size(); ++i)
    {
        vertices.emplace_back(Vertex(points.verts[i], points.normals[i]));
        indices.emplace_back(static_cast<uint32>(points.indices[i]));
    }
    return MakeShared<VoxelEngine::components::mesh::Mesh>(vertices, indices);
}

vmodel::Model CreateTestSphereModel(const double& radius, const glm::ivec3 gridSize)
{
    return vmodel::Sphere(radius, gridSize / 2, gridSize);
}

vmodel::Model CreateTestPawnModel(const double& scaleFactor, const glm::ivec3& gridSize)
{
    vmodel::Model osh = vmodel::OneSheetedHyperboloid(2 * scaleFactor, 2 * scaleFactor, 4 * scaleFactor, {25 * scaleFactor, 30 * scaleFactor, 25 * scaleFactor}, Direction::OY, gridSize);
    vmodel::Model doublePlane = vmodel::DoublePlane(12 * scaleFactor, {25 * scaleFactor, 22 * scaleFactor, 25 * scaleFactor}, Direction::OY, gridSize);
    vmodel::Model sum1 = vmodel::Operations::sum(osh, doublePlane, 1);
        
    vmodel::Model ellipsoid = vmodel::Ellipsoid(4.5 * scaleFactor, 1.4 * scaleFactor, 4.5 * scaleFactor, {25 * scaleFactor, 30 * scaleFactor, 25 * scaleFactor}, gridSize);
    vmodel::Model comp1 = vmodel::Operations::comp(sum1, ellipsoid, 1);
        
    vmodel::Model sphere = vmodel::Sphere(3.0 * scaleFactor, {25 * scaleFactor, 33 * scaleFactor, 25 * scaleFactor}, gridSize);
    vmodel::Model comp2 = vmodel::Operations::comp(comp1, sphere, 1);

    return comp2;
}
