#include "editor_extension.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

using namespace Editor;
using namespace Engine;

Engine::EResourceDescription::ResBuffer ResMesh::ImportMesh(const Engine::EResourceDescription::RawBuffer data)
{
    EResourceDescription::ResBuffer result;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFileFromMemory(data.Data, data.Size, aiProcess_Triangulate | aiProcess_GenSmoothNormals);
    

    return result;
}
