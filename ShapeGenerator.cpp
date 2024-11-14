// ShapeGenerator.cpp

#include "ShapeGenerator.h"
#include "Mesh.h"

void ShapeGenerator::CreatePyramid(std::vector<Mesh::Vertex>& vertices, std::vector<UINT>& indices) {
    // Vertex array
    vertices = {
        // Base vertices
        { -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f }, // 0 Left-back
        {  0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f }, // 1 Right-back
        {  0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f }, // 2 Right-front
        { -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f }, // 3 Left-front
        // Apex vertex
        {  0.0f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f }  // 4 Top vertex
    };

    // Index array for drawing the pyramid
    indices = {
        // Base (correct as per your information)
        0, 1, 2,
        0, 2, 3,
        // Sides (reorder for correct normals)
        4, 1, 0,  // Side 1
        4, 2, 1,  // Side 2
        4, 3, 2,  // Side 3
        4, 0, 3   // Side 4
    };
}
