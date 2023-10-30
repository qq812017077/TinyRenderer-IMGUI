#pragma once
#include "object/GameObject.h"
#include "EngineMath.h"
#include <memory>

class Primitive 
{
public:

    static GameObject* CreateSimpleCube(std::string name);
    static GameObject* CreateSkinedCube(std::string name);
    static GameObject* CreateCube(std::string name);
    static GameObject* CreatePlane(std::string name);
    static GameObject* CreateBox(std::string name, float width, float height, float depth);

    static std::shared_ptr<Mesh> CreateQuadMeshPtr();
    static Mesh CreateQuadMesh();
    static Mesh CreateCameraWireframeMesh();
    static Mesh CreateCameraFrustumMesh(float fov,float aspect,float nearPlane,float farPlane);
    static Mesh CreateCubeMesh(float scale = 1.0f);
};