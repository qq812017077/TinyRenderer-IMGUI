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


GameObject* Primitive::CreateSphere(std::string name, int sector, int stack)
{
    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Vector2> texCoords;
    constexpr float radius = 1.0f;
    
    auto base = Vector4(0.0f, 0.0f, radius, 0);
    float longDiv = sector;
    float latDiv = stack;
    const float lattitudeAngle = PI / latDiv;
    const float longitudeAngle = 2.0f * PI / longDiv;
    for( int iLat = 1; iLat < latDiv; iLat++ )
    {
        //rotate around X axis
        auto latBase = Matrix4x4::RotationX( lattitudeAngle * iLat ) * base;
        for( int iLong = 0; iLong < longDiv; iLong++ )
        {
            //rotate around Y axis
            auto v = Matrix4x4::RotationZ( longitudeAngle * iLong ) * latBase;
            vertices.push_back( Vector3( v.x, v.y, v.z ) );
            normals.push_back( Vector3( v.x, v.y, v.z ) );
            texCoords.push_back( Vector2( (float)iLong / longDiv, (float)iLat / latDiv ) );
        }
    }

    // cap vertices
    const auto iNorthPole = (unsigned short)vertices.size();
    {
        Vector3 northPos = base;
        vertices.push_back( northPos );
    }
    const auto iSouthPole = (unsigned short)vertices.size();
    {
        Vector3 southPos = Vector3::zero - base;
        vertices.push_back( southPos );
    }

    const auto calcIdx = [latDiv,longDiv]( unsigned short iLat,unsigned short iLong )
			{ return iLat * longDiv + iLong; };
        
    std::vector<unsigned short> indices;
    for( unsigned short iLat = 0; iLat < latDiv - 2; iLat++ )
    {
        for( unsigned short iLong = 0; iLong < longDiv - 1; iLong++ )
        {
            indices.push_back( calcIdx( iLat,iLong ) );
            indices.push_back( calcIdx( iLat + 1,iLong ) );
            indices.push_back( calcIdx( iLat,iLong + 1 ) );
            indices.push_back( calcIdx( iLat,iLong + 1 ) );
            indices.push_back( calcIdx( iLat + 1,iLong ) );
            indices.push_back( calcIdx( iLat + 1,iLong + 1 ) );
        }
        // wrap band
        indices.push_back( calcIdx( iLat,longDiv - 1 ) );
        indices.push_back( calcIdx( iLat + 1,longDiv - 1 ) );
        indices.push_back( calcIdx( iLat,0 ) );
        indices.push_back( calcIdx( iLat,0 ) );
        indices.push_back( calcIdx( iLat + 1,longDiv - 1 ) );
        indices.push_back( calcIdx( iLat + 1,0 ) );			
    }

    // cap fans
    for( unsigned short iLong = 0; iLong < longDiv - 1; iLong++ )
    {
        // north
        indices.push_back( iNorthPole );
        indices.push_back( calcIdx( 0,iLong ) );
        indices.push_back( calcIdx( 0,iLong + 1 ) );
        // south
        indices.push_back( calcIdx( latDiv - 2,iLong + 1 ) );
        indices.push_back( calcIdx( latDiv - 2,iLong ) );
        indices.push_back( iSouthPole );
    }
    // wrap triangles
    // north
    indices.push_back( iNorthPole );
    indices.push_back( calcIdx( 0,longDiv - 1 ) );
    indices.push_back( calcIdx( 0,0 ) );
    // south
    indices.push_back( calcIdx( latDiv - 2,0 ) );
    indices.push_back( calcIdx( latDiv - 2,longDiv - 1 ) );
    indices.push_back( iSouthPole );

    Mesh mesh;
    mesh.SetVertexPosition(vertices);
    mesh.SetVertexIndices(indices);
    mesh.SetVertexTexCoord(texCoords);
    mesh.SetVertexNormal(normals);

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


Mesh Primitive::CreateCameraWireframeMesh()
{
    const float x = 4.0f / 3.0f * 0.75f;
    const float y = 1.0f * 0.75f;
    const float z = -2.0f;
    const float thalf = x * 0.5f;
    const float tspace = y * 0.2f;
    std::vector<Vector3> position = {
        { -x,y,0.0f }, // 0
        { x,y,0.0f }, // 1
        { x,-y,0.0f }, // 2
        { -x,-y,0.0f }, // 3
        { 0.0f,0.0f,z }, // 4
        { -thalf,y + tspace,0.0f }, // 5
        { thalf,y + tspace,0.0f }, // 6
        { 0.0f,y + tspace + thalf,0.0f } // 7
    };

    std::vector<INDICE_TYPE> indices = {
        0,1, 1,2, 2,3, 3,0,
        0,4, 1,4, 2,4, 3,4,
        5,6, 6,7, 7,5
    };

    Mesh mesh;
    mesh.SetVertexPosition(position);
    mesh.SetVertexIndices(indices);
    return mesh; 
}


Mesh Primitive::CreateCameraFrustumMesh(float fov,float aspect,float nearPlane,float farPlane)
{
    float fovRad = fov * 0.5f * PI / 180.0f;
    float nearY = tanf(fovRad) * nearPlane;
    float nearX = nearY * aspect;
    float farY = tanf(fovRad) * farPlane;
    float farX = farY * aspect;

    std::vector<Float3> position = {
        { nearX, nearY, nearPlane},
        { -nearX, nearY, nearPlane},
        { -nearX, -nearY, nearPlane},
        { nearX, -nearY, nearPlane},
        { farX, farY, farPlane},
        { -farX, farY, farPlane},
        { -farX, -farY, farPlane},
        { farX, -farY, farPlane}
    };

    std::vector<INDICE_TYPE> indices = {
        0,1, 1,2, 2,3, 3,0,
        0,4, 1,5, 2,6, 3,7,
        4,5, 5,6, 6,7, 7,4
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