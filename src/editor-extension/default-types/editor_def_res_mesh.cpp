#include "editor_extension.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

using namespace Editor;
using namespace Engine;

void EMeshResource::FromBuffer(Engine::ESharedBuffer buffer)
{
    Vertices.clear();
    Indices.clear();

#ifdef E_BUILD_ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFileFromMemory(buffer.Data(), buffer.GetSizeInByte(), aiProcess_Triangulate | aiProcess_GenNormals);

    if(!scene)
    {
        return;
    }
    u32 indexOffset = 0;
    
    u32 numMeshes = scene->mNumMeshes;
    for (u32 i = 0; i < numMeshes; i++)
    {
        const aiMesh* mesh = scene->mMeshes[i];
        
        // Load vertices
        u32 numVertices = mesh->mNumVertices;
        for (u32 j = 0; j < numVertices; j++)
        {
            const aiVector3D& v = mesh->mVertices[j];
            const aiVector3D& n = mesh->mNormals[j];
            const aiVector3D& u = mesh->mTextureCoords[0][j];
            Vertices.push_back({EVec3(v.x, v.y, v.z), EVec3(n.x, n.y, n.z)});
        }

        // Load indices
        u32 numFaces = mesh->mNumFaces;
        for (u32 j = 0; j < numFaces; j++)
        {
            const aiFace& face = mesh->mFaces[j];
            u32 numIndices = face.mNumIndices;
            if (numIndices != 3)
            {
                E_WARN("Only triangles are supported");
                continue;
            }
            Indices.push_back(face.mIndices[0] + indexOffset);
            Indices.push_back(face.mIndices[1] + indexOffset);
            Indices.push_back(face.mIndices[2] + indexOffset);
        }
        indexOffset += numVertices;
    }
#endif
}


Engine::EResourceDescription::ResBuffer ResMesh::ImportMesh(const Engine::EResourceDescription::RawBuffer data)
{
    EResourceDescription::ResBuffer result;
    

    return result;
}
