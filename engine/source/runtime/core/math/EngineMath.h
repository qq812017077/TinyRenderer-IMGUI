#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
const float PI = 3.14159265358979323846f;
const static float kEpsilon = 1E-06f;
const static float kRad2Deg = 57.29578f;
const static float kDeg2Rad = 0.01745329f;

struct Float2
{
    float x;
    float y;
};
struct Float3
{
    float x;
    float y;
    float z;
    Float3() = default;
    Float3(float x, float y, float z) : x(x), y(y), z(z) {}
};
struct Float4
{
    float x;
    float y;
    float z;
    float w;
};

struct ColorRGBA
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    ColorRGBA() = default;
    ColorRGBA(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b), a(255) {}
    ColorRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r(r), g(g), b(b), a(a) {}

};
