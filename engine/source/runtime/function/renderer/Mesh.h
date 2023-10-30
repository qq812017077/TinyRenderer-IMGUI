#pragma once
#include <vector>
#include "EngineMath.h"
#include <string>
#include "geometry/Bounds.h"

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
    ~Mesh();    
    Mesh(Mesh const& other);
    Mesh& operator=(Mesh& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    // Set Functions
    void SetVertexIndices(std::vector<uint16_t> indices);
    void SetVertexIndices(std::vector<uint32_t> indices);
    void SetVertexPosition(std::vector<Float3> positions);
    void SetVertexPosition(std::vector<Float2> positions);
    void SetVertexNormal(std::vector<Float3> normals);
    void SetVertexColor(std::vector<ColorRGBA> colors);
    void SetVertexTexCoord(std::vector<Float2> texCoords);
    void SetVertexTangent(std::vector<Float3> tangents);
    void SetVertexBinormal(std::vector<Float3> binormals);
    bool HasVertexDataType(VertexDataType vertexType);

    // Get Functions
    void* GetVertexBuffer(const char * semanticName, unsigned int & bufSize);
    
    unsigned int GetVertexCount();
    void * GetVertexBufferAddress();
    unsigned int GetVertexBufferSize();
    void * GetIndexBufferAddress();
    unsigned int GetIndexBufferSize();
    unsigned int GetVertexStride();
    unsigned int GetIndexStride();
    unsigned int GetIndexCount();

    Bounds GetBounds();
    // static VertexDataType GetVertexDataType(const char* semanticName);
    // static unsigned int GetAlignedByteOffset(const char* semanticName, int semanticIndex = 0);
    
    enum PositionType
    {
        Position3D,
        Position2D
    };
    std::vector<Vertex> vertices;
    std::vector<Float3> positions; 
    std::vector<Float3> normals;
    std::vector<ColorRGBA> colors;
    std::vector<Float2> texCoords;
    std::vector<Float3> tangents;
    std::vector<Float3> binormals;
    Bounds bounds;
    
    std::vector<Float2> positions2D; 
private:
    enum INDICE_TYPE
    {
        UINT16,
        UINT32
    };
    VertexDataType availableVertexType = VertexDataType::None;
    // vertex array
    // index array
    PositionType posType = PositionType::Position3D;
    INDICE_TYPE indiceType = INDICE_TYPE::UINT16;
    std::vector<uint16_t> indices_16;
    std::vector<uint32_t> indices_32;
};
