#include "resources/ModelLoader.h"
#include "core/Log.h"
#include <string> 
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// TRUE OR FALSE DID MESH FILE LOAD MESH CHECK
bool ModelLoader::LoadMesh(const std::string& filepath, LoadedMeshData& outData) {
    Assimp::Importer importer;

    // CONVERTS ALL POLYGONS IN YOUR 3D MODEL FILE INTO TRIANGLES STORES INSIDE aiScene STRUCT IN assimp
    const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);

    // CHECK: file fail print message 
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        ENGINE_ERROR("Assimp Model Parsing Error: " + std::string(importer.GetErrorString()));
        return false;
    }
    // CHECK: empty file print fail 
    if (scene->mNumMeshes == 0) {
        ENGINE_ERROR("Model loader error: No meshes found inside file " + filepath);
        return false;
    }

    // GRABS FIRST ACTUAL 3D OBJECT INSIDE MESH MODEL FILE 
    aiMesh* mesh = scene->mMeshes[0];
    // ALLOCATE MEMORY 
    outData.Vertices.reserve(mesh->mNumVertices);

    // EXTRACT VERTEX ATTRIBUTES(X, Y, Z + U, V)
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        ModelVertex vertex;

        // Positions
        vertex.Position.x = mesh->mVertices[i].x;
        vertex.Position.y = mesh->mVertices[i].y;
        vertex.Position.z = mesh->mVertices[i].z;

        // Texture UVs
        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        // PACKS MESH DATA TO VERTEX
        outData.Vertices.push_back(vertex);
    }

    // EXTRACT FACES INDEX MAP
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            outData.Indices.push_back(face.mIndices[j]);
        }
    }
    // MESSAGE SUCCESS
    ENGINE_INFO("Assimp Model parsed successfully from: " + filepath +
        " [Vertices: " + std::to_string(outData.Vertices.size()) +
        ", Indices: " + std::to_string(outData.Indices.size()) + "]");

    return true;
}