#pragma once
#include <vector>
#include "EngineMath.h"
#include <string>

#define INDICE_TYPE unsigned short
enum class VertexDataType
{
    None     = 0,
    Position    = 1 << 0,
    Position2D  = 1 << 1,
    
    Normal      = 1 << 5,
    Color       = 1 << 10,
    TexCoord    = 1 << 15,
    Tangent     = 1 << 20,
    Binormal    = 1 << 25
};

class Mesh
{
    
public:
    struct Vertex { 
        Float3 position; 
        Float3 normal;
        ColorRGBA color;
        Float2 texCoord;
        Float3 tangent;
        Float3 binormal;

        Vertex() = default;
        Vertex(float positionX, float positionY, float positionZ):
            position(positionX, positionY, positionZ)
        {
        }
    };

    Mesh();
    Mesh(std::string filePath);
    ~Mesh();    
    Mesh(Mesh const& other);
    Mesh& operator=(Mesh& other) noexcept;

    // Set Functions
    void SetVertexIndices(std::vector<INDICE_TYPE> indices);
    void SetVertexPosition(std::vector<Float3> positions);
    void SetVertexNormal(std::vector<Float3> normals);
    void SetVertexColor(std::vector<ColorRGBA> colors);
    void SetVertexTexCoord(std::vector<Float2> texCoords);
    void SetVertexTangent(std::vector<Float3> tangents);

    bool HasVertexDataType(VertexDataType vertexType);

    // Get Functions
    unsigned int GetVertexCount();
    
    void * GetVertexBufferAddress();
    unsigned int GetVertexBufferSize();
    void * GetIndexBufferAddress();
    unsigned int GetIndexBufferSize();
    unsigned int GetVertexStride();
    unsigned int GetIndexStride();
    unsigned int GetIndexCount();
    static VertexDataType GetVertexDataType(const char* semanticName);
    static unsigned int GetAlignedByteOffset(const char* semanticName, int semanticIndex = 0);

private:
    VertexDataType availableVertexType = VertexDataType::None;
    // vertex array
    std::vector<Vertex> vertices;
    // index array
    std::vector<INDICE_TYPE> indices;
};