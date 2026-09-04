#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

// VERTEX STRUCT BUNDLE OUR 3D DATA PER POINT 
struct ModelVertex {
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal;
};
// HOLD FINAL COMPILED MESH ARRAYS
struct LoadedMeshData {
    std::vector<ModelVertex> Vertices;
    std::vector<uint32_t> Indices;
};
class ModelLoader {
public:
    // EXTRA MODEL FILE GEOMETRIC INFO SO YOUR ENGINE CAN RENDER IT
    static bool LoadMesh(const std::string& filepath, LoadedMeshData& outData);
};