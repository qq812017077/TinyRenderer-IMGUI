#pragma once
#include "GameObject.h"
#include "EngineMath.h"

class Cube : public GameObject
{
public:
    Cube();
    Cube(std::string name);
    Cube(const Cube& other) = delete;
    ~Cube();

    void SetCenter(const Vector3& center);
    void SetSize(const Vector3& size);
    void OnUpdate(float deltaTime) override;

private:

    Vector3 center;
    Vector3 size;
};