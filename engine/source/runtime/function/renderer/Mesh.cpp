#include "Mesh.h"
#ifdef _DEBUG
#include <iostream>
#endif
#include "core/EngineException.h"
#define STREQUAL(a, b) (_stricmp(a, b) == 0)

inline VertexDataType operator|(VertexDataType lhs, VertexDataType rhs)
{
    using T = std::underlying_type_t<VertexDataType>;
    return static_cast<VertexDataType>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

inline VertexDataType operator|=(VertexDataType lhs, VertexDataType rhs)
{
    using T = std::underlying_type_t<VertexDataType>;
    return static_cast<VertexDataType>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

inline VertexDataType operator&(VertexDataType lhs, VertexDataType rhs)
{
    using T = std::underlying_type_t<VertexDataType>;
    return static_cast<VertexDataType>(static_cast<T>(lhs) & static_cast<T>(rhs));
}
Mesh::Mesh()
{
    availableVertexType = VertexDataType::None;
}

Mesh::Mesh(std::string filePath)
{
    
}

Mesh::~Mesh()
{

}

Mesh::Mesh(Mesh const& other) : 
    availableVertexType(other.availableVertexType),
    vertices(other.vertices), 
    indices(other.indices)
{

}

Mesh& Mesh::operator=(Mesh& other) noexcept
{
    vertices = other.vertices;
    indices = other.indices;
    return *this;
}

// Set Functions
void Mesh::SetVertexIndices(std::vector<INDICE_TYPE> indices)
{
    this->indices = indices;
}

void Mesh::SetVertexPosition(std::vector<Float3> positions)
{
    vertices = std::vector<Vertex>(positions.size());
    availableVertexType |= VertexDataType::Position;
    for (int i = 0; i < positions.size(); i++)
    {
        vertices[i].position = positions[i];
    }
}

void Mesh::SetVertexNormal(std::vector<Float3> normals)
{
    availableVertexType |= VertexDataType::Normal;
    for (int i = 0; i < normals.size(); i++)
    {
        vertices[i].normal = normals[i];
    }
}

void Mesh::SetVertexColor(std::vector<ColorRGBA> colors)
{
    availableVertexType |= VertexDataType::Color;
    for (int i = 0; i < colors.size(); i++)
    {
        vertices[i].color = colors[i];
    }
}

void Mesh::SetVertexTexCoord(std::vector<Float2> texCoords)
{
    availableVertexType |= VertexDataType::TexCoord;
    for (int i = 0; i < texCoords.size(); i++)
    {
        vertices[i].texCoord = texCoords[i];
    }
}

void Mesh::SetVertexTangent(std::vector<Float3> tangents)
{
    availableVertexType |= VertexDataType::Tangent;
    for (int i = 0; i < tangents.size(); i++)
    {
        vertices[i].tangent = tangents[i];
    }
}

bool Mesh::HasVertexDataType(VertexDataType vertexType)
{
    return (availableVertexType & vertexType) == vertexType;
}

// Get Functions

unsigned int Mesh::GetVertexCount()
{
    return static_cast<unsigned int>(vertices.size());
}

void * Mesh::GetVertexBufferAddress()
{
    return vertices.data();
}

unsigned int Mesh::GetVertexBufferSize()
{
    return static_cast<unsigned int>(vertices.size() * sizeof(Vertex));
}

void * Mesh::GetIndexBufferAddress()
{
    return indices.data();
}

unsigned int Mesh::GetIndexBufferSize()
{
    return static_cast<unsigned int>(indices.size() * sizeof(INDICE_TYPE));
}

unsigned int Mesh::GetVertexStride()
{
    return sizeof(Vertex);
}

unsigned int Mesh::GetIndexStride()
{
    return sizeof(INDICE_TYPE);
}

unsigned int Mesh::GetIndexCount()
{
    return static_cast<unsigned int>(indices.size());
}
VertexDataType Mesh::GetVertexDataType(const char* semanticName)
{
    // string compartion in c type
    if(STREQUAL(semanticName, "Position")) return VertexDataType::Position;
    else if( STREQUAL(semanticName, "Normal")) return VertexDataType::Normal;
    else if( STREQUAL(semanticName, "Color")) return VertexDataType::Color;
    else if( STREQUAL(semanticName, "Texcoord")) return VertexDataType::TexCoord;
    else if( STREQUAL(semanticName, "Tangent")) return VertexDataType::Tangent;
    else if( STREQUAL(semanticName, "Binormal")) return VertexDataType::Binormal;
    else return VertexDataType::Position;
}


unsigned int Mesh::GetAlignedByteOffset(const char * semanticName, int semanticIndex)
{
    if(STREQUAL(semanticName, "Position")) return offsetof(Vertex, position);
    else if(STREQUAL(semanticName, "NORMAL")) return offsetof(Vertex, normal);
    else if(STREQUAL(semanticName, "Color")) return offsetof(Vertex, color);
    else if(STREQUAL(semanticName, "Texcoord")) return offsetof(Vertex, texCoord);
    else if(STREQUAL(semanticName, "Tangent")) return offsetof(Vertex, tangent);
    else if(STREQUAL(semanticName, "Binormal")) return offsetof(Vertex, binormal);
    #ifdef _DEBUG
        std::cout << "unknown semantic name: " << semanticName << " in " << __LINE__ << __FILE__ << std::endl;
    #endif
    throw EngineException(__LINE__, __FILE__, ("unknown semantic name:" + std::string(semanticName)).c_str());
    return 0;

}