// ShapeGenerator.cpp

#include "ShapeGenerator.h"
#include "Mesh.h"

void ShapeGenerator::CreatePyramid(std::vector<Mesh::Vertex>& vertices, std::vector<UINT>& indices) {
    // Define the pyramid vertices
    vertices = {
        // Base vertices
        { -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f },  // Left-back (red)
        {  0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f },  // Right-back (green)
        {  0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f },  // Right-front (blue)
        { -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f },  // Left-front (yellow)
        // Apex vertex
        {  0.0f,  0.5f,  0.0f,  1.0f, 0.0f, 1.0f }   // Top vertex (magenta)
    };

    // Define the indices for the pyramid
    indices = {
        // Base
        0, 1, 2,
        0, 2, 3,
        // Sides
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };
}
