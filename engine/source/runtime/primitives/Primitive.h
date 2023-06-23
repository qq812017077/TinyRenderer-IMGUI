#pragma once
#include "GameObject.h"
#include "EngineMath.h"

class Primitive : public GameObject
{
protected:
    Primitive();
    Primitive(std::string name);
    Primitive(const Primitive& other) = delete;
    ~Primitive();

public:
    void Primitive::Init() override;
    virtual std::vector<Float3> GetVerticesPosition() = 0;
    virtual std::vector<INDICE_TYPE> GetVerticesIndices() = 0;
    virtual std::vector<ColorRGBA> GetVerticesColor() = 0;

};