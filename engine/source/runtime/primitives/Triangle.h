#pragma once
#include "primitives/Primitive.h"

class Triangle : public Primitive
{
public:
    Triangle() : Primitive("Triangle") {}
    ~Triangle() = default;


    virtual std::vector<Float3> GetVerticesPosition() override
    {
        return {
        { 0.0f, 0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        { -0.5f, -0.5f, 0.0f},
        { -0.3f, 0.3f, 0.0f},
        { 0.3f, 0.3f, 0.0f},
        { 0.0f, -0.8f, 0.0f}
        };
    }

    virtual std::vector<INDICE_TYPE> GetVerticesIndices() override
    {
        return {
        0, 1, 2,
        0, 2, 3,
        0, 4, 1,
        2, 1, 5
        };
    }

    virtual std::vector<ColorRGBA> GetVerticesColor() override
    {
        return {
        { 0, 0, 255 }, // 0
        { 0, 255, 0 }, // 1
        { 255, 0, 0 }, // 2
        { 255, 0, 255 }, // 3
        { 255, 255, 0 }, // 4
        { 0, 255, 255 }, // 5
        };
    }
    
    void OnUpdate(float deltaTime) override
    {
        auto rotation = transform.GetRotation();
        rotation.z = rotation.z + deltaTime * 20.f;
        transform.SetRotation(rotation);
    }
};