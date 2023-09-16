#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
const float PI = 3.14159265358979323846f;
const static float kEpsilon = 1E-06f;
const static float kRad2Deg = 57.29578f;
const static float kDeg2Rad = 0.01745329f;

typedef Vector3 Float3;
typedef Vector4 Float4;
typedef Vector2 Float2;
struct ColorRGBA
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    ColorRGBA() = default;
    ColorRGBA(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b), a(255) {}
    ColorRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r(r), g(g), b(b), a(a) {}
    ColorRGBA(int r, int g, int b) : r(static_cast<unsigned char>(r)), g(static_cast<unsigned char>(g)), b(static_cast<unsigned char>(b)), a(255) {}
    ColorRGBA(int r, int g, int b, int a) : r(static_cast<unsigned char>(r)), g(static_cast<unsigned char>(g)), b(static_cast<unsigned char>(b)), a(static_cast<unsigned char>(a)) {}
    ColorRGBA(float r, float g, float b) :
        r(static_cast<unsigned char>(r * 255)),
        g(static_cast<unsigned char>(g * 255)),
        b(static_cast<unsigned char>(b * 255)),
        a(255) {}
    ColorRGBA(float r, float g, float b, float a) :
        r(static_cast<unsigned char>(r * 255)),
        g(static_cast<unsigned char>(g * 255)),
        b(static_cast<unsigned char>(b * 255)),
        a(static_cast<unsigned char>(a * 255)) {}
};
