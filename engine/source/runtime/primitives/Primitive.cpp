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
    AddComponent<Renderer>(mesh);
}


