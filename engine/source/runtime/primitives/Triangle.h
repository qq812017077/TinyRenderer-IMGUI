#pragma once
#include "GameObject.h"
#include "EngineMath.h"

class Triangle : public GameObject
{
public:
    Triangle();
    Triangle(std::string name);
    Triangle(const Triangle& other) = delete;
    ~Triangle();

    void OnUpdate(float deltaTime) override;

private:
};