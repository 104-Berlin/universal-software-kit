#include "engine_shared.h"
#include "prefix_ecs.h"

using namespace Engine;


EScene::EScene(const EString& name)
    : fName("Name", name), 
    fSelectedObject("SelectedObject")
{
    LoadDefaultMeshes();
}

EScene::~EScene()
{
    std::cout << "Scene \"" << fName.GetValue() << "\" Deleted!" << std::endl;
}

EResourceManager& EScene::GetResourceManager() 
{
    return fResourceManager;
}

void EScene::Update(float delta)
{
}

EObjectRef& EScene::GetSelectedObject() 
{
    return fSelectedObject;
}

EObject EScene::GetObjectByUuid(const EUUID& uuid)
{
    return fEntityMap[uuid];
}

void EScene::SetJsObject(EJson& json)
{
    fName.SetJsObject(json);
    EJson& objectArray = json["Objects"] = EJson::array();
    ForEachObject([&objectArray](EObject object) mutable {
        EJson objectJson = EJson::object();
        object.SetJsObject(objectJson);
        objectArray.push_back(objectJson);
    });
}

void EScene::FromJsObject(const EJson& json) 
{
    fName.FromJsObject(json);

    const EJson& objectArray = json["Objects"];
    if (objectArray.is_array())
    {
        fRegistry.clear();
        for (const EJson& objectJson : objectArray)
        {
            EObject newObject = CreateObject();
            newObject.FromJsObject(objectJson);
        }
    } 
}

void EScene::Clear() 
{
    fRegistry.clear();
}

void EScene::ForEachObject(ObjectCallback fn)
{
    fRegistry.each([this, fn](EEntity entity){
        this->CallObjectFunc(entity, fn);
    }); 
}

void EScene::CallObjectFunc(EEntity entity, ObjectCallback fn) 
{
    fn(EObject(entity, this));
}

EObject EScene::CreateObject()
{
    EEntity handle = fRegistry.create();
    EObject object(handle, this);

    // Make sure we dont have the uuid registered allready
    EUUID newUuid = EUUID().CreateNew();
    while (fEntityMap.find(newUuid) != fEntityMap.end())
    {
        newUuid = EUUID().CreateNew();
    }
    // -----------------------------------


    object.AddComponent<ETagComponent>("Empty Object", newUuid);
    object.AddComponent<ETransformComponent>();
    fEntityMap.insert({ newUuid, object });
    return object;
}





// ------------------------------------------------------------
// Default meshes

static EVector<EMesh::EVertex> vertices = {
    { {-1,  1, 1},         {0.0f, 0.0f, 1.0f}, {0, 1}},
    {{-1, -1, 1},         {0.0f, 0.0f, 1.0f}, {0, 0}},
    {{ 1, -1, 1},         {0.0f, 0.0f, 1.0f}, {1, 0}},
    {{ 1,  1, 1},         {0.0f, 0.0f, 1.0f}, {1, 1}},
    
    {{-1,  1, -1},        {0.0f, 0.0f, -1.0f}, {0, 1}},
    {{-1, -1, -1},        {0.0f, 0.0f, -1.0f}, {0, 0}},
    {{ 1, -1, -1},        {0.0f, 0.0f, -1.0f}, {1, 0}},
    {{ 1,  1, -1},        {0.0f, 0.0f, -1.0f}, {1, 1}},
    
    
    
    {{1, -1,  1},          {1.0f, 0.0f, 0.0f}, {0, 1}},
    {{1, -1, -1},          {1.0f, 0.0f, 0.0f}, {0, 0}},
    {{1,  1, -1},          {1.0f, 0.0f, 0.0f}, {1, 0}},
    {{1,  1,  1},          {1.0f, 0.0f, 0.0f}, {1, 1}},

    {{-1, -1,  1},         {-1.0f, 0.0f, 1.0f}, {0, 1}},
    {{-1, -1, -1},         {-1.0f, 0.0f, 1.0f}, {0, 0}},
    {{-1,  1, -1},         {-1.0f, 0.0f, 1.0f}, {1, 0}},
    {{-1,  1,  1},         {-1.0f, 0.0f, 1.0f}, {1, 1}},


    {{-1,  1,  1},         {0.0f, 1.0f, 0.0f}, {0, 1}},
    {{-1,  1, -1},         {0.0f, 1.0f, 0.0f}, {0, 0}},
    {{ 1,  1, -1},         {0.0f, 1.0f, 0.0f}, {1, 0}},
    {{ 1,  1,  1},         {0.0f, 1.0f, 0.0f}, {1, 1}},

    {{-1, -1,  1},         {0.0f, -1.0f, 0.0f}, {0, 1}},
    {{-1, -1, -1},         {0.0f, -1.0f, 0.0f}, {0, 0}},
    {{ 1, -1, -1},         {0.0f, -1.0f, 0.0f}, {1, 0}},
    {{ 1, -1,  1},         {0.0f, -1.0f, 0.0f}, {1, 1}},
};

static EVector<u32> indices = {
    0, 1, 2,
    2, 3, 0,

    4, 5, 6,
    6, 7, 4,

    8, 9, 10,
    10, 11, 8,

    12, 13, 14,
    14, 15, 12,

    16, 17, 18,
    18, 19, 16,

    20, 21, 22,
    22, 23, 20        
};

static EVector<EMesh::EVertex> vertices_2 = {
    {{-0.5f, -0.5f, -1.0f}, {0, 0, 0}, {0, 0}},
    {{-0.5f,  0.5f, -1.0f}, {0, 0, 0}, {0, 0}},
    {{ 0.5f,  0.5f, -1.0f}, {0, 0, 0}, {0, 0}},
    {{ 0.5f, -0.5f, -1.0f}, {0, 0, 0}, {0, 0}},
};

static EVector<u32> indices_2 = {
    0, 1, 2,
    2, 3, 0
};

static EVector<EMesh::EVertex> vertices_3 = {
    {{-0.5f, -0.5f, -1.0f}, {0, 0, 0}, {0, 0}},
    {{ 0.0f,  0.5f, -1.0f}, {0, 0, 0}, {0, 0}},
    {{ 0.5f, -0.5f, -1.0f}, {0, 0, 0}, {0, 0}},
};

static EVector<u32> indices_3 = {
    0, 1, 2,
};

void EScene::LoadDefaultMeshes()
{
    GetResourceManager().AddLoadedResource(EMakeRef(EMesh, "Cube", vertices, indices));
    GetResourceManager().AddLoadedResource(EMakeRef(EMesh, "Plane", vertices_2, indices_2));
    GetResourceManager().AddLoadedResource(EMakeRef(EMesh, "Dreieck", vertices_3, indices_3));
}