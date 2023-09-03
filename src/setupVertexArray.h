#pragma once
#include <vector>
#include "Face.h"
#include <glm/glm.hpp>

std::vector<glm::vec3> setupVertexArray(const std::vector<glm::vec3>& vertices, const std::vector<Face>& faces, const std::vector<glm::vec3>& normals)
{
    std::vector<glm::vec3> vertexArray;
    
    // For each face
    for (const auto& face : faces)
    {
        // For each vertex in the face
        for (int i = 0; i < 3; ++i)
        {
            // Get the vertex position and normal from the input arrays using the indices from the face
            glm::vec3 vertexPosition(vertices[face.vertexIndices[i]]);
            glm::vec3 vertexNormal = normals[face.normalIndices[i]];

            // Add the vertex position and normal to the vertex array
            vertexArray.push_back(vertexPosition);
            vertexArray.push_back(vertexNormal);
        }
    }

    return vertexArray;
}