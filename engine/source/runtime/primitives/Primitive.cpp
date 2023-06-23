#include "Primitive.h"

Primitive::Primitive():
    Primitive("New Game Object")
{

}

Primitive::Primitive(std::string name):
    GameObject(name)
{
    
}
Primitive::~Primitive()
{
}

void Primitive::Init()
{
    
    Mesh mesh;
    mesh.SetVertexPosition(GetVerticesPosition());
    mesh.SetVertexIndices(GetVerticesIndices());
    mesh.SetVertexColor(GetVerticesColor());
    SetRenderer(std::make_unique<Renderer>(*this, mesh));
}

// std::vector<Float3> Primitive::GetVerticesPosition()
// {
//     return std::vector<Float3>();
// }

// std::vector<INDICE_TYPE> Primitive::GetVerticesIndices()
// {
//     return std::vector<INDICE_TYPE>();
// }

// std::vector<ColorRGBA> Primitive::GetVerticesColor()
// {
//     return std::vector<ColorRGBA>();
// }



