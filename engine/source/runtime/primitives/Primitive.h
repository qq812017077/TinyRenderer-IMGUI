#pragma once
#include "core/GameObject.h"
#include "EngineMath.h"
#include <memory>

class Primitive 
{
public:

    static std::unique_ptr<GameObject> CreateSimpleCube(std::string name);
    static std::unique_ptr<GameObject> CreateSkinedCube(std::string name);
    static std::unique_ptr<GameObject> CreateCube(std::string name);
    static std::unique_ptr<GameObject> CreatePlane(std::string name);

    static std::unique_ptr<GameObject> CreateBox(std::string name, float width, float height, float depth);

};