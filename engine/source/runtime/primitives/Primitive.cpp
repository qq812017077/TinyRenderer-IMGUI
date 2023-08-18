#include "Primitive.h"


std::unique_ptr<GameObject> Primitive::CreateSimpleCube(std::string name)
{
    std::vector<Float3> position = {
        { -1.0f, -1.0f, -1.0f }, // 0
        { -1.0f,  1.0f, -1.0f }, // 1
        {  1.0f,  1.0f, -1.0f }, // 2
        {  1.0f, -1.0f, -1.0f }, // 3
        { -1.0f, -1.0f,  1.0f }, // 4
        { -1.0f,  1.0f,  1.0f }, // 5
        {  1.0f,  1.0f,  1.0f }, // 6
        {  1.0f, -1.0f,  1.0f }  // 7
        };
    std::vector<INDICE_TYPE> indices = {
        0, 1, 2,
        2, 3, 0, // front 
        4, 5, 1,
        1, 0, 4, // left
        1, 5, 6,
        6, 2, 1, // top
        7, 6, 5,
        5, 4, 7, // back
        3, 2, 6,
        6, 7, 3, // left
        4, 0, 3,
        3, 7, 4  // bottom
        };
    std::vector<ColorRGBA> colors = {
        { 0, 0, 255 }, // 0
        { 0, 255, 0 }, // 1
        { 255, 0, 0 }, // 2
        { 0, 255, 255 }, // 3
        { 0, 0, 255 }, // 4
        { 255, 0, 0 }, // 5
        { 0, 255, 0 }, // 6
        { 255, 255, 0 }  // 7
        };
    std::vector<Float2> uvs = {
        { 0.0f, 0.0f }, // 0
        { 0.0f, 1.0f }, // 1
        { 1.0f, 1.0f }, // 2
        { 1.0f, 0.0f }, // 3
        { 0.0f, 0.0f }, // 4
        { 0.0f, 1.0f }, // 5
        { 1.0f, 1.0f }, // 6
        { 1.0f, 0.0f }  // 7
        };
    
    Mesh mesh;
    mesh.SetVertexPosition(position);
    mesh.SetVertexIndices(indices);
    mesh.SetVertexColor(colors);
    mesh.SetVertexTexCoord(uvs);

    std::unique_ptr<GameObject> pGO = std::make_unique<GameObject>(name);
    pGO->AddComponent<Renderer>(mesh);
    return pGO;
}

std::unique_ptr<GameObject> Primitive::CreateSkinedCube(std::string name)
{
    constexpr float side = 1.0f;
    Mesh mesh;
    mesh.vertices.resize(14);
    mesh.vertices[0].position = { -side,-side,-side };
    mesh.vertices[0].texCoord = { 2.0f / 3.0f,0.0f / 4.0f };
    mesh.vertices[1].position = { side,-side,-side };
    mesh.vertices[1].texCoord = { 1.0f / 3.0f,0.0f / 4.0f };
    mesh.vertices[2].position = { -side,side,-side };
    mesh.vertices[2].texCoord = { 2.0f / 3.0f,1.0f / 4.0f };
    mesh.vertices[3].position = { side,side,-side };
    mesh.vertices[3].texCoord = { 1.0f / 3.0f,1.0f / 4.0f };
    mesh.vertices[4].position = { -side,-side,side };
    mesh.vertices[4].texCoord = { 2.0f / 3.0f,3.0f / 4.0f };
    mesh.vertices[5].position = { side,-side,side };
    mesh.vertices[5].texCoord = { 1.0f / 3.0f,3.0f / 4.0f };
    mesh.vertices[6].position = { -side,side,side };
    mesh.vertices[6].texCoord = { 2.0f / 3.0f,2.0f / 4.0f };
    mesh.vertices[7].position = { side,side,side };
    mesh.vertices[7].texCoord = { 1.0f / 3.0f,2.0f / 4.0f };
    mesh.vertices[8].position = { -side,-side,-side };
    mesh.vertices[8].texCoord = { 2.0f / 3.0f,4.0f / 4.0f };
    mesh.vertices[9].position = { side,-side,-side };
    mesh.vertices[9].texCoord = { 1.0f / 3.0f,4.0f / 4.0f };
    mesh.vertices[10].position = { -side,-side,-side };
    mesh.vertices[10].texCoord = { 3.0f / 3.0f,1.0f / 4.0f };
    mesh.vertices[11].position = { -side,-side,side };
    mesh.vertices[11].texCoord = { 3.0f / 3.0f,2.0f / 4.0f };
    mesh.vertices[12].position = { side,-side,-side };
    mesh.vertices[12].texCoord = { 0.0f / 3.0f,1.0f / 4.0f };
    mesh.vertices[13].position = { side,-side,side };
    mesh.vertices[13].texCoord = { 0.0f / 3.0f,2.0f / 4.0f };
    
    std::vector<INDICE_TYPE> indices = {
                0,2,1,   2,3,1,
				4,8,5,   5,8,9,
				2,6,3,   3,6,7,
				4,5,7,   4,7,6,
				2,10,11, 2,11,6,
				12,3,7,  12,7,13
    };
    mesh.SetVertexIndices(indices);
    std::unique_ptr<GameObject> pGO = std::make_unique<GameObject>(name);
    pGO->AddComponent<Renderer>(mesh);
    return pGO;
}

std::unique_ptr<GameObject> Primitive::CreateCube(std::string name)
{
    constexpr float side = 1.0f;
    std::vector<Float3> position = {
            { -side,-side,-side },// 0 near side
            { side,-side,-side },// 1
            { -side,side,-side },// 2
            { side,side,-side },// 3
            { -side,-side,side },// 4 far side
            { side,-side,side },// 5
            { -side,side,side },// 6
            { side,side,side },// 7
            { -side,-side,-side },// 8 left side
            { -side,side,-side },// 9
            { -side,-side,side },// 10
            { -side,side,side },// 11
            { side,-side,-side },// 12 right side
            { side,side,-side },// 13
            { side,-side,side },// 14
            { side,side,side },// 15
            { -side,-side,-side },// 16 bottom side
            { side,-side,-side },// 17
            { -side,-side,side },// 18
            { side,-side,side },// 19
            { -side,side,-side },// 20 top side
            { side,side,-side },// 21
            { -side,side,side },// 22
            { side,side,side }// 23
        };
    std::vector<INDICE_TYPE> indices = {
                0,2, 1,    2,3,1,
				4,5, 7,    4,7,6,
				8,10, 9,  10,11,9,
				12,13,15, 12,15,14,
				16,17,18, 18,17,19,
				20,23,21, 20,22,23
        };
        
    std::vector<Float2> uvs = {
        { 0.0f,0.0f },
        { 1.0f,0.0f },
        { 0.0f,1.0f },
        { 1.0f,1.0f },
        { 0.0f,0.0f },
        { 1.0f,0.0f },
        { 0.0f,1.0f },
        { 1.0f,1.0f },
        { 0.0f,0.0f },
        { 1.0f,0.0f },
        { 0.0f,1.0f },
        { 1.0f,1.0f },
        { 0.0f,0.0f },
        { 1.0f,0.0f },
        { 0.0f,1.0f },
        { 1.0f,1.0f },
        { 0.0f,0.0f },
        { 1.0f,0.0f },
        { 0.0f,1.0f },
        { 1.0f,1.0f },
        { 0.0f,0.0f },
        { 1.0f,0.0f },
        { 0.0f,1.0f },
        { 1.0f,1.0f }
    };
    
    Mesh mesh;
    mesh.SetVertexPosition(position);
    mesh.SetVertexIndices(indices);
    mesh.SetVertexTexCoord(uvs);

    std::unique_ptr<GameObject> pGO = std::make_unique<GameObject>(name);
    pGO->AddComponent<Renderer>(mesh);
    return pGO;
}