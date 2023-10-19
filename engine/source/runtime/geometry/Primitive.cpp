#include "Primitive.h"
#include "Color.h"
typedef unsigned short INDICE_TYPE;
GameObject* Primitive::CreateSimpleCube(std::string name)
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

    GameObject* pGO = GameObject::CreateGameObject(name);
    pGO->AddComponent<Renderer>(mesh);
    return pGO;
}

GameObject* Primitive::CreateSkinedCube(std::string name)
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
    GameObject* pGO = GameObject::CreateGameObject(name);
    pGO->AddComponent<Renderer>(mesh);
    return pGO;
}

GameObject* Primitive::CreateCube(std::string name)
{
    auto cubeMesh = CreateCubeMesh();
    auto pGO = GameObject::CreateGameObject(name);
    auto renderer = pGO->AddComponent<Renderer>(cubeMesh);
    return pGO;
}

GameObject* Primitive::CreatePlane(std::string name)
{
    float size = 10.f;
    std::vector<Float3> position = {
        { -size, 0.0f, -size }, // 0
        { -size, 0.0f, size }, // 1
        { size, 0.0f, size }, // 2
        { size, 0.0f, -size } // 3
    };

    std::vector<Float3> normals = {
        { 0.0f, 1.0f, 0.0f }, // 0
        { 0.0f, 1.0f, 0.0f }, // 1
        { 0.0f, 1.0f, 0.0f }, // 2
        { 0.0f, 1.0f, 0.0f } // 3
    };

    std::vector<Float2> uvs = {
        { 0.0f, 0.0f }, // 0
        { 0.0f, 1.0f }, // 1
        { 1.0f, 1.0f }, // 2
        { 1.0f, 0.0f } // 3
    };

    std::vector<INDICE_TYPE> indices = {
        0, 1, 2,
        2, 3, 0
    };

    Mesh mesh;
    mesh.SetVertexPosition(position);
    mesh.SetVertexIndices(indices);
    mesh.SetVertexTexCoord(uvs);
    mesh.SetVertexNormal(normals);

    GameObject* pGO = GameObject::CreateGameObject(name);
    pGO->AddComponent<Renderer>(mesh);
    return pGO;
}
GameObject* Primitive::CreateBox(std::string name, float width, float height, float depth)
{
    float halfWidth = width / 2.0f, halfHeight = height / 2.0f, halfDepth = depth / 2.0f;
    Mesh mesh;
    mesh.vertices.resize(24);   
    mesh.vertices[0].position = {halfWidth, -halfHeight, -halfDepth}; // +x
    mesh.vertices[1].position = {halfWidth, halfHeight, -halfDepth};
    mesh.vertices[2].position = {halfWidth, halfHeight, halfDepth};
    mesh.vertices[3].position = {halfWidth, -halfHeight, halfDepth};

    mesh.vertices[4].position = {-halfWidth, -halfHeight, -halfDepth}; // -x
    mesh.vertices[5].position = {-halfWidth, halfHeight, -halfDepth};
    mesh.vertices[6].position = {-halfWidth, halfHeight, halfDepth};
    mesh.vertices[7].position = {-halfWidth, -halfHeight, halfDepth};
    
    mesh.vertices[8].position = {-halfWidth, halfHeight, halfDepth}; // +y
    mesh.vertices[9].position = {halfWidth, halfHeight, halfDepth};
    mesh.vertices[10].position = {halfWidth, halfHeight, -halfDepth};
    mesh.vertices[11].position = {-halfWidth, halfHeight, -halfDepth};

    mesh.vertices[12].position = {-halfWidth, -halfHeight, halfDepth}; // -y
    mesh.vertices[13].position = {halfWidth, -halfHeight, halfDepth};
    mesh.vertices[14].position = {halfWidth, -halfHeight, -halfDepth};
    mesh.vertices[15].position = {-halfWidth, -halfHeight, -halfDepth};

    mesh.vertices[16].position = {-halfWidth, -halfHeight, halfDepth}; // +z
    mesh.vertices[17].position = {-halfWidth, halfHeight, halfDepth};
    mesh.vertices[18].position = {halfWidth, halfHeight, halfDepth};
    mesh.vertices[19].position = {halfWidth, -halfHeight, halfDepth};

    mesh.vertices[20].position = {-halfWidth, -halfHeight, -halfDepth}; // -z
    mesh.vertices[21].position = {-halfWidth, halfHeight, -halfDepth};
    mesh.vertices[22].position = {halfWidth, halfHeight, -halfDepth};
    mesh.vertices[23].position = {halfWidth, -halfHeight, -halfDepth};

    for (int i = 0; i < 4; ++i)
    {
        // +x
        mesh.vertices[i].normal = { 1.0f, 0.0f, 0.0f };
        mesh.vertices[i].tangent = { 0.0f, 0.0f, 1.0f};

        // -x
        mesh.vertices[i + 4].normal = { -1.0f, 0.0f, 0.0f };
        mesh.vertices[i + 4].tangent = { 0.0f, 0.0f, -1.0f };

        // +y
        mesh.vertices[i + 8].normal = { 0.0f, 1.0f, 0.0f };
        mesh.vertices[i + 8].tangent = { 1.0f, 0.0f, 0.0f };

        // -y
        mesh.vertices[i + 12].normal = { 0.0f, -1.0f, 0.0f };
        mesh.vertices[i + 12].tangent = { -1.0f, 0.0f, 0.0f };

        // +z
        mesh.vertices[i + 16].normal = { 0.0f, 0.0f, 1.0f };
        mesh.vertices[i + 16].tangent = { 1.0f, 0.0f, 0.0f };

        // -z
        mesh.vertices[i + 20].normal = { 0.0f, 0.0f, -1.0f };
        mesh.vertices[i + 20].tangent = { -1.0f, 0.0f, 0.0f };
    }

    for (int i = 0; i < 6; ++i)
    {
        mesh.vertices[i * 4].texCoord = { 0.0f, 0.0f };
        mesh.vertices[i * 4 + 1].texCoord = { 1.0f, 0.0f };
        mesh.vertices[i * 4 + 2].texCoord = { 1.0f, 1.0f };
        mesh.vertices[i * 4 + 3].texCoord = { 0.0f, 1.0f };
    }

    std::vector<INDICE_TYPE> indices = {
        0, 1, 2,
        2, 3, 0, // +x
        4, 5, 6,
        6, 7, 4, // -x
        8, 9, 10,
        10, 11, 8, // +y
        12, 13, 14,
        14, 15, 12, // -y
        16, 17, 18,
        18, 19, 16, // +z
        20, 21, 22,
        22, 23, 20 // -z
    };
    mesh.SetVertexIndices(indices);
    GameObject* pGO = GameObject::CreateGameObject(name);
    pGO->AddComponent<Renderer>(mesh);
    return pGO;
}


std::shared_ptr<Mesh> Primitive::CreateQuadMeshPtr()
{
    auto pMesh = std::make_shared<Mesh>();
    
    float size = 1;
    std::vector<Float2> position = {
        { -size, size }, // 0
        { size,  size }, // 1
        { -size, -size}, // 2
        { size,  -size} // 3
    };

    std::vector<INDICE_TYPE> indices = {
        0,1,2,
        1,3,2
    };

    pMesh->SetVertexPosition(position);
    pMesh->SetVertexIndices(indices);
    return pMesh;
}

Mesh Primitive::CreateQuadMesh()
{
    float size = 1;
    std::vector<Float2> position = {
        { -size, size }, // 0
        { size,  size }, // 1
        { -size, -size}, // 2
        { size,  -size} // 3
    };

    std::vector<INDICE_TYPE> indices = {
        0,1,2,
        1,3,2
    };

    Mesh mesh;
    mesh.SetVertexPosition(position);
    mesh.SetVertexIndices(indices);
    return mesh;
}

Mesh Primitive::CreateCubeMesh(float scale)
{
    const float side = 1.0f * scale;
    std::vector<Float3> position = {
            { side, -side, side }, // 0
            { -side, -side, side}, // 1
            { side, side , side},  // 2

            { -side, side, side},   // 3
            { side,  side, -side },
            { -side, side, -side},

            { side, -side, -side},
            { -side, -side, -side},
            { side, side, side},

            { -side, side, side},
            { side, side, -side},
            { -side, side, -side},

            { side, -side, -side}, // 12
            { side, -side, side},
            { -side, -side, side},
            
            { -side, -side, -side}, // 15
            { -side, -side , side},
            { -side, side , side},

            { -side, side , -side}, //18
            { -side, -side , -side},
            { side, -side , -side},

            { side, side , -side}, //21
            { side, side , side},
            { side, -side , side},
        };
    std::vector<INDICE_TYPE> indices = {
                0,2, 3,    0,3,1,
				8,4, 5,    8,5,9,
				10,6, 7,  10,7,11,
				12,13,14, 12,14,15,
				16,17,18, 16,18,19,
				20,21,22, 20,22,23
        };
        
    std::vector<Float2> uvs = {
        { 0.0f,0.0f },  //0
        { 1.0f,0.0f },
        { 0.0f,1.0f },

        { 1.0f,1.0f },  //3
        { 0.0f,1.0f },
        { 1.0f,1.0f },
        
        { 0.0f,1.0f },  //6
        { 1.0f,1.0f },
        { 0.0f,0.0f },
        
        { 1.0f,0.0f },  //9
        { 0.0f,0.0f },
        { 1.0f,0.0f },
        
        { 0.0f,0.0f },  //12
        { 0.0f,1.0f },
        { 1.0f,1.0f },
        
        { 1.0f,0.0f },  //15
        { 0.0f,0.0f },
        { 0.0f,1.0f },
        
        { 1.0f,1.0f },  //18
        { 1.0f,0.0f },
        { 0.0f,0.0f },
        
        { 0.0f,1.0f },  //21
        { 1.0f,1.0f },
        { 1.0f,0.0f }
    };
    // normals for every vertex: 6 faces, 4 vertices per face
    std::vector<Float3> normals =
    {
        { 0.0f,0.0f, 1.0f },// 0 near
        { 0.0f,0.0f, 1.0f },
        { 0.0f,0.0f, 1.0f },

        { 0.0f,0.0f, 1.0f },
        { 0.0f,1.0f,0.0f },// 4 far
        { 0.0f,1.0f,0.0f },
        
        { 0.0f,0.0f,-1.0f },
        { 0.0f,0.0f,-1.0f },
        { 0.0f,1.0f,0.0f },// 8 left

        { 0.0f,1.0f,0.0f },
        { 0.0f,0.0f,-1.0f },
        { 0.0f,0.0f,-1.0f },

        { 0.0f,-1.0f,0.0f },// 12 right
        { 0.0f,-1.0f,0.0f },
        { 0.0f,-1.0f,0.0f },

        { 0.0f,-1.0f,0.0f },
        { -1.0f,0.0f,0.0f },// 16 bottom
        { -1.0f,0.0f,0.0f },

        { -1.0f,0.0f,0.0f },
        { -1.0f,0.0f,0.0f },
        { 1.0f,0.0f,0.0f },// 20 top

        { 1.0f,0.0f,0.0f },
        { 1.0f,0.0f,0.0f },
        { 1.0f,0.0f,0.0f } 
    };
    Mesh mesh;
    mesh.SetVertexPosition(position);
    mesh.SetVertexIndices(indices);
    mesh.SetVertexTexCoord(uvs);
    mesh.SetVertexNormal(normals);
    return mesh;
}