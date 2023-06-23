#pragma once
#include "Primitive.h"

// ******************
// 设置立方体顶点
//    5________ 6
//    /|      /|
//   /_|_____/ |                y
//  1|4|_ _ 2|_|7               |  z
//   | /     | /                | /
//   |/______|/                 |/____x
//  0       3
class Cube : public Primitive
{
public:
    Cube() : Primitive("Cube") {}
    ~Cube() = default;
    
    std::vector<Float3> GetVerticesPosition() override
    {
        return {
        { -1.0f, -1.0f, -1.0f }, // 0
        { -1.0f,  1.0f, -1.0f }, // 1
        {  1.0f,  1.0f, -1.0f }, // 2
        {  1.0f, -1.0f, -1.0f }, // 3
        { -1.0f, -1.0f,  1.0f }, // 4
        { -1.0f,  1.0f,  1.0f }, // 5
        {  1.0f,  1.0f,  1.0f }, // 6
        {  1.0f, -1.0f,  1.0f }  // 7
        };
    }

    std::vector<INDICE_TYPE> GetVerticesIndices() override
    {
        return {
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
    }

    std::vector<ColorRGBA> GetVerticesColor() override
    {
        return {
        { 0, 0, 255 }, // 0
        { 0, 255, 0 }, // 1
        { 255, 0, 0 }, // 2
        { 0, 255, 255 }, // 3
        { 0, 0, 255 }, // 4
        { 255, 0, 0 }, // 5
        { 0, 255, 0 }, // 6
        { 255, 255, 0 }  // 7
        };
    }

    void OnUpdate(float deltaTime) override
    {
        // add random rotation
        transform.Rotate({ 0.0f, 0.0f, 1.0f }, deltaTime * 15.f);
        transform.Rotate({ 0.0f, 1.0f, 0.0f }, deltaTime * 15.f);
        transform.Rotate({ 1.0f, 0.0f, 0.0f }, deltaTime * 15.f);
    }
private:
    Vector3 center;
    Vector3 size;
};