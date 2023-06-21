#include "Cube.h"

Cube::Cube():
    Cube("New Game Object")
{

}

Cube::Cube(std::string name):
    GameObject(name)
{
    Mesh mesh;
    //TODO: cube mesh vertices and indices
    std::vector<Float3> cube_vertices_position{
        { -1.0f, -1.0f, -1.0f }, // 0
        { -1.0f, -1.0f,  1.0f }, // 1
        { -1.0f,  1.0f, -1.0f }, // 2
        { -1.0f,  1.0f,  1.0f }, // 3
        {  1.0f, -1.0f, -1.0f }, // 4
        {  1.0f, -1.0f,  1.0f }, // 5
        {  1.0f,  1.0f, -1.0f }, // 6
        {  1.0f,  1.0f,  1.0f }  // 7
    };

    // cube indices
    std::vector<INDICE_TYPE> cube_vertices_indices{
        0, 1, 2, 
        2, 1, 3, // -x
        4, 6, 5, 
        5, 6, 7, // +x
        0, 4, 1, 
        1, 4, 5, // -y
        2, 3, 6, 
        6, 3, 7, // +y
        0, 2, 4, 
        4, 2, 6, // -z
        1, 5, 3, 
        3, 5, 7  // +z
    };

    // Cube color
    std::vector<ColorRGBA> cube_vertices_colors{
        { 0, 0, 255 }, // 0
        { 0, 255, 0 }, // 1
        { 255, 0, 0 }, // 2
        { 0, 255, 255 }, // 3
        { 0, 0, 255 }, // 4
        { 255, 0, 0 }, // 5
        { 0, 255, 0 }, // 6
        { 255, 255, 0 }  // 7
    };
    
    mesh.SetVertexPosition(cube_vertices_position);
    mesh.SetVertexIndices(cube_vertices_indices);
    mesh.SetVertexColor(cube_vertices_colors);
    
    SetRenderer(std::make_unique<Renderer>(*this, mesh));
}

Cube::~Cube()
{
}

void Cube::SetCenter(const Vector3& center)
{
    this->center = center;
    transform.SetPosition(center);
}

void Cube::SetSize(const Vector3& size)
{
    this->size = size;
    transform.SetScale(size);
}

void Cube::OnUpdate(float deltaTime)
{
    //std::cout << "Cube::OnUpdate" << std::endl;
    // print deltaTime
    std::cout << "deltaTime: " << deltaTime << std::endl;
}


