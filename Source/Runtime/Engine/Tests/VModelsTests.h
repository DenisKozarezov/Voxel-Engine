#pragma once
#include "Core/CoreDefines.h"
#include "VModel.h"

// SPHERE
        // Model sphere = new Sphere(16, new Vector3(25, 25, 25), gridSize);
        
        // TORUS
        // Model torus = new Torus(15, 10, new Vector3(25, 25, 25), gridSize);
        
        // PLANE
        // Model plane = new VModel.Plane(surfPoints[0].position, surfPoints[1].position, surfPoints[2].position, gridSize);
        
        // ELLIPSOID
        // Model ellipsoid = new Ellipsoid(15, 10, 20, new Vector3(25, 25, 25), gridSize);
        
        // CYLINDER
        // Model cylinder = new Cylinder(10, 10, new Vector3(25, 25, 25), Direction.Oy, gridSize);
        // Model doublePlane = new DoublePlane(12, new Vector3(25, 25, 25), Direction.Oy, gridSize);
        // var cyl = Sum(cylinder, doublePlane, 1);
        
        // CUBE
        // var side = 16;
        // Model doublePlane1 = new DoublePlane(side, new Vector3(25, 25, 25), Direction.Ox, gridSize);
        // Model doublePlane2 = new DoublePlane(side, new Vector3(25, 25, 25), Direction.Oy, gridSize);
        // Model doublePlane3 = new DoublePlane(side, new Vector3(25, 25, 25), Direction.Oz, gridSize);
        // var cube = Sum(doublePlane3, Sum(doublePlane1, doublePlane2, 1), 1);
        
        // CONE
        // Model cone = new Cone(6, 6, 6, new Vector3(25, 25, 25), Direction.Oy, gridSize);
        
        // PAWN
        //Model pawn = GetPawn();
        
        // SUM
        // Model sum = Sum(plane, NewInvert(torus), 1);
        
        // COMP
        // Model comp = Comp(sphere, torus, 1);
        
        // HOMOTOPY
        // Model homotopy = Homotopy(cyl, pawn, 0d);

        // var points = comp.GetPoints();

namespace VoxelEngine::components::mesh
{
        struct Mesh;
}

TSharedPtr<VoxelEngine::components::mesh::Mesh> CreateTestPawn(const glm::ivec3 gridSize = {200, 200, 200});

TSharedPtr<VoxelEngine::components::mesh::Mesh> CreateTestHomotopy(const glm::ivec3 gridSize = {200, 200, 200});

vmodel::Model CreateTestSphereModel(const double& radius, const glm::ivec3 gridSize = {200, 200, 200});

vmodel::Model CreateTestPawnModel(const double& scaleFactor, const glm::ivec3& gridSize = {200, 200, 200});



