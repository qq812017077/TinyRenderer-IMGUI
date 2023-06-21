#include "Triangle.h"


Triangle::Triangle():
    Triangle("Triangle")
{

}

Triangle::Triangle(std::string name):
    GameObject(name)
{
    Mesh mesh;
    //TODO: cube mesh vertices and indices
    std::vector<Float3> vertices_position{
        { 0.0f, 0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        { -0.5f, -0.5f, 0.0}
    };
    
    // cube indices
    std::vector<INDICE_TYPE> vertices_indices{
        0, 1, 2,
    };

    // Cube color
    std::vector<ColorRGBA> vertices_colors{
        { 0, 0, 255 }, // 0
        { 0, 255, 0 }, // 1
        { 255, 0, 0 }, // 2
    };
    
    mesh.SetVertexPosition(vertices_position);
    mesh.SetVertexIndices(vertices_indices);
    mesh.SetVertexColor(vertices_colors);
    
    SetRenderer(std::make_unique<Renderer>(*this, mesh));
}

Triangle::~Triangle()
{
}


void Triangle::OnUpdate(float deltaTime)
{
    std::cout << "deltaTime: " << deltaTime << std::endl;
}


