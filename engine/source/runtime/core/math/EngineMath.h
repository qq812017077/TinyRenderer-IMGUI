#pragma once

#include "Vector.h"
#include "Matrix4x4.h"
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
