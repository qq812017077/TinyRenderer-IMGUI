#include "Mesh.h"
#ifdef _DEBUG
#include <iostream>
#endif
#include <cassert>
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
    indiceType = INDICE_TYPE::UINT16;
}

Mesh::~Mesh()
{

}

Mesh::Mesh(Mesh const& other) : 
    availableVertexType(other.availableVertexType),
    vertices(other.vertices), 
    indices_16(other.indices_16),
    indices_32(other.indices_32),
    bounds(other.bounds),
    positions(other.positions),
    normals(other.normals),
    colors(other.colors),
    texCoords(other.texCoords),
    tangents(other.tangents),
    binormals(other.binormals)
{
    indiceType = other.indiceType;
}

Mesh& Mesh::operator=(Mesh& other) noexcept
{
    availableVertexType = other.availableVertexType;
    vertices = other.vertices;
    indices_16 = other.indices_16;
    indices_32 = other.indices_32;
    indiceType = other.indiceType;
    positions = other.positions;
    normals = other.normals;
    colors = other.colors;
    texCoords = other.texCoords;
    tangents = other.tangents;
    binormals = other.binormals;
    bounds = other.bounds;
    return *this;
}

// Set Functions
void Mesh::SetVertexIndices(std::vector<uint16_t> indices)
{
    this->indices_16 = indices;
    this->indiceType = INDICE_TYPE::UINT16;
}
void Mesh::SetVertexIndices(std::vector<uint32_t> indices)
{
    this->indices_32 = indices;
    this->indiceType = INDICE_TYPE::UINT32;
}
void Mesh::SetVertexPosition(std::vector<Float3> positions)
{
    
    this->positions = positions;
    vertices = std::vector<Vertex>(positions.size());
    availableVertexType |= VertexDataType::Position;
    Vector3 minPos = positions[0];
    Vector3 maxPos = positions[0];
    for (int i = 0; i < positions.size(); i++)
    {
        vertices[i].position = positions[i];
        minPos = Vector3::Min(minPos, positions[i]);
        maxPos = Vector3::Max(maxPos, positions[i]);
    }
    // update bounds
    bounds.SetMinMax(minPos, maxPos);

}

void Mesh::SetVertexNormal(std::vector<Float3> normals)
{
    this->normals = normals;
    availableVertexType |= VertexDataType::Normal;
    for (int i = 0; i < normals.size(); i++)
    {
        vertices[i].normal = normals[i];
    }
}

void Mesh::SetVertexColor(std::vector<ColorRGBA> colors)
{
    this->colors = colors;
    availableVertexType |= VertexDataType::Color;
    for (int i = 0; i < colors.size(); i++)
    {
        vertices[i].color = colors[i];
    }
}

void Mesh::SetVertexTexCoord(std::vector<Float2> texCoords)
{
    this->texCoords = texCoords;
    availableVertexType |= VertexDataType::TexCoord;
    for (int i = 0; i < texCoords.size(); i++)
    {
        vertices[i].texCoord = texCoords[i];
    }
}

void Mesh::SetVertexTangent(std::vector<Float3> tangents)
{
    this->tangents = tangents;
    availableVertexType |= VertexDataType::Tangent;
    for (int i = 0; i < tangents.size(); i++)
    {
        vertices[i].tangent = tangents[i];
    }
}

void Mesh::SetVertexBinormal(std::vector<Float3> binormals)
{
    this->binormals = binormals;
    availableVertexType |= VertexDataType::Binormal;
    for (int i = 0; i < binormals.size(); i++)
    {
        vertices[i].binormal = binormals[i];
    }
}
bool Mesh::HasVertexDataType(VertexDataType vertexType)
{
    return (availableVertexType & vertexType) == vertexType;
}

// Get Functions
void* Mesh::GetVertexBuffer(const char * semanticName, unsigned int & bufSize)
{
    if(STREQUAL(semanticName, "Position")) 
    {
        bufSize = static_cast<unsigned int>(positions.size() * sizeof(Float3));
        return positions.data();
    }
    else if( STREQUAL(semanticName, "Normal"))
    {
        bufSize = static_cast<unsigned int>(normals.size() * sizeof(Float3));
        return normals.data();
    }
    else if( STREQUAL(semanticName, "Color"))
    {
        bufSize = static_cast<unsigned int>(colors.size() * sizeof(ColorRGBA));
        return colors.data();
    }
    else if( STREQUAL(semanticName, "Texcoord"))
    {
        bufSize = static_cast<unsigned int>(texCoords.size() * sizeof(Float2));
        return texCoords.data();
    }
    else if( STREQUAL(semanticName, "Tangent"))
    {
        bufSize = static_cast<unsigned int>(tangents.size() * sizeof(Float3));
        return tangents.data();
    }
    else if( STREQUAL(semanticName, "Binormal"))
    {
        bufSize = static_cast<unsigned int>(binormals.size() * sizeof(Float3));
        return binormals.data();
    }
    
    #ifdef _DEBUG
        std::cout << "unknown semantic name: " << semanticName << " in " << __LINE__ << __FILE__ << std::endl;
    #endif

    throw EngineException(__LINE__, __FILE__, ("unknown semantic name:" + std::string(semanticName)).c_str());
    return nullptr;
}
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
    return (indiceType == INDICE_TYPE::UINT16) ? 
        reinterpret_cast<void*>(indices_16.data()) :
        reinterpret_cast<void*>(indices_32.data());
}

unsigned int Mesh::GetIndexBufferSize()
{
    size_t idxBufSize = (indiceType == INDICE_TYPE::UINT16)? 
                (indices_16.size() * sizeof(uint16_t)) : 
                (indices_32.size() * sizeof(uint32_t));
    return static_cast<unsigned int>(idxBufSize);
    // if(indiceType == INDICE_TYPE::UINT16)
    //     return indices_16.size() * sizeof(uint16_t);
    // return indices_32.size() * sizeof(uint32_t);
}

unsigned int Mesh::GetVertexStride()
{
    return sizeof(Vertex);
}

unsigned int Mesh::GetIndexStride()
{
    size_t idxStride = (indiceType == INDICE_TYPE::UINT16)? sizeof(uint16_t) : sizeof(uint32_t);
    return static_cast<unsigned int>(idxStride);
}

unsigned int Mesh::GetIndexCount()
{
    size_t idxCount = (indiceType == INDICE_TYPE::UINT16)? indices_16.size() : indices_32.size();
    return static_cast<unsigned int>(idxCount);
}

Bounds Mesh::GetBounds()
{
    return bounds;
}

// VertexDataType Mesh::GetVertexDataType(const char* semanticName)
// {
//     // string compartion in c type
//     if(STREQUAL(semanticName, "Position")) return VertexDataType::Position;
//     else if( STREQUAL(semanticName, "Normal")) return VertexDataType::Normal;
//     else if( STREQUAL(semanticName, "Color")) return VertexDataType::Color;
//     else if( STREQUAL(semanticName, "Texcoord")) return VertexDataType::TexCoord;
//     else if( STREQUAL(semanticName, "Tangent")) return VertexDataType::Tangent;
//     else if( STREQUAL(semanticName, "Binormal")) return VertexDataType::Binormal;
//     else return VertexDataType::Position;
// }


// unsigned int Mesh::GetAlignedByteOffset(const char * semanticName, int semanticIndex)
// {
//     if(STREQUAL(semanticName, "Position")) return offsetof(Vertex, position);
//     else if(STREQUAL(semanticName, "NORMAL")) return offsetof(Vertex, normal);
//     else if(STREQUAL(semanticName, "Color")) return offsetof(Vertex, color);
//     else if(STREQUAL(semanticName, "Texcoord")) return offsetof(Vertex, texCoord);
//     else if(STREQUAL(semanticName, "Tangent")) return offsetof(Vertex, tangent);
//     else if(STREQUAL(semanticName, "Binormal")) return offsetof(Vertex, binormal);
//     #ifdef _DEBUG
//         std::cout << "unknown semantic name: " << semanticName << " in " << __LINE__ << __FILE__ << std::endl;
//     #endif
//     throw EngineException(__LINE__, __FILE__, ("unknown semantic name:" + std::string(semanticName)).c_str());
//     return 0;

// }
