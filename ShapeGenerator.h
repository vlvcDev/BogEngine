#pragma once
#include "Mesh.h"

class ShapeGenerator {
public:
    static void CreatePyramid(std::vector<Mesh::Vertex>& vertices, std::vector<UINT>& indices);
    static void CreateBox(std::vector<Mesh::Vertex>& vertices, std::vector<UINT>& indices);
    static void CreateSphere(std::vector<Mesh::Vertex>& vertices, std::vector<UINT>& indices, float radius, UINT sliceCount, UINT stackCount);
    static void CreatePlane(std::vector<Mesh::Vertex>& vertices, std::vector<UINT>& indices, float width, float depth);
};
